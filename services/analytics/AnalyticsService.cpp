/**
 * @file AnalyticsService.cpp
 * @brief Real-time analytics microservice.
 *
 * Consumes order events from Kafka, computes running statistics,
 * and exposes analytics data via metrics.
 */

#include <Service.h>

class AnalyticsService : public rbsys::Service {
public:
    std::string name() const override { return "AnalyticsService"; }

    void onStart() override {
        log().info("AnalyticsService starting");

        // Subscribe to Kafka order events
        subscribe("order-events", [this](const Message& msg) {
            metrics().requestStarted();
            auto start = std::chrono::steady_clock::now();

            ++m_eventsProcessed;

            std::string payload(
                reinterpret_cast<const char*>(msg.payload.data()), // NOLINT
                msg.payload.size());

            if (m_eventsProcessed % 100 == 0) {
                log().info("Analytics milestone: " + std::to_string(m_eventsProcessed) +
                           " events processed");
            }

            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start);
            metrics().recordRequest(latency, 200);
            metrics().requestCompleted();
        });

        log().info("AnalyticsService ready — subscribed to order-events (Kafka)");
    }

    void onStop() override {
        log().info("AnalyticsService stopping, total events=" +
                   std::to_string(m_eventsProcessed));
    }

private:
    int64_t m_eventsProcessed{0};
};

RBSYS_SERVICE(AnalyticsService) // NOLINT
