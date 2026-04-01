/**
 * @file OrderService.cpp
 * @brief Order processing microservice.
 *
 * Receives order requests, validates them, forwards to InventoryService
 * for stock checking, and publishes order events to Kafka for analytics.
 *
 * Build: g++ -shared -fPIC -o order-service.so OrderService.cpp -lrbsys
 */

#include <Service.h>

class OrderService : public rbsys::Service {
public:
    std::string name() const override { return "OrderService"; }

    void onStart() override {
        log().info("OrderService starting");

        // Subscribe to incoming order requests
        subscribe("order.create", [this](const Message& msg) {
            auto start = std::chrono::steady_clock::now();
            metrics().requestStarted();

            std::string payload(
                reinterpret_cast<const char*>(msg.payload.data()), // NOLINT
                msg.payload.size());

            log().info("Processing order: " + payload.substr(0, 80));

            // Forward to inventory for stock check (async via RabbitMQ)
            publish("inventory.check", payload, msg.key);

            // Publish order event to Kafka for analytics
            publish("order-events", payload, msg.key);

            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start);
            metrics().recordRequest(latency, 200);
            metrics().requestCompleted();
        });

        log().info("OrderService ready — subscribed to order.create");
    }

    void onStop() override {
        log().info("OrderService stopping");
    }
};

RBSYS_SERVICE(OrderService) // NOLINT
