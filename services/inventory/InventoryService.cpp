/**
 * @file InventoryService.cpp
 * @brief Inventory management microservice.
 *
 * Checks stock levels, reserves inventory for orders,
 * and forwards confirmed orders to NotificationService.
 */

#include <Service.h>

class InventoryService : public rbsys::Service {
public:
    std::string name() const override { return "InventoryService"; }

    void onStart() override {
        log().info("InventoryService starting with stock=" + std::to_string(m_stock));

        subscribe("inventory.check", [this](const Message& msg) {
            metrics().requestStarted();
            auto start = std::chrono::steady_clock::now();

            std::string payload(
                reinterpret_cast<const char*>(msg.payload.data()), // NOLINT
                msg.payload.size());

            if (m_stock > 0) {
                --m_stock;
                log().info("Stock reserved (" + std::to_string(m_stock) +
                           " remaining): " + payload.substr(0, 60));

                // Forward to notification
                publish("notify.send", "CONFIRMED|" + payload);
            } else {
                log().warn("Out of stock! Resetting to 100");
                m_stock = 100;
            }

            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start);
            metrics().recordRequest(latency, 200);
            metrics().requestCompleted();
        });

        log().info("InventoryService ready — subscribed to inventory.check");
    }

    void onStop() override {
        log().info("InventoryService stopping, final stock=" + std::to_string(m_stock));
    }

private:
    int m_stock{100};
};

RBSYS_SERVICE(InventoryService) // NOLINT
