/**
 * @file NotificationService.cpp
 * @brief Customer notification microservice.
 *
 * Receives confirmed order events and sends notifications
 * (email, SMS, push — simulated via logging).
 */

#include <Service.h>

class NotificationService : public rbsys::Service {
public:
    std::string name() const override { return "NotificationService"; }

    void onStart() override {
        log().info("NotificationService starting");

        subscribe("notify.send", [this](const Message& msg) {
            metrics().requestStarted();
            auto start = std::chrono::steady_clock::now();

            std::string payload(
                reinterpret_cast<const char*>(msg.payload.data()), // NOLINT
                msg.payload.size());

            ++m_sent;
            log().info("Notification #" + std::to_string(m_sent) +
                       " sent: " + payload.substr(0, 80));

            auto latency = std::chrono::duration_cast<std::chrono::microseconds>(
                std::chrono::steady_clock::now() - start);
            metrics().recordRequest(latency, 200);
            metrics().requestCompleted();
        });

        log().info("NotificationService ready — subscribed to notify.send");
    }

    void onStop() override {
        log().info("NotificationService stopping, total sent=" + std::to_string(m_sent));
    }

private:
    int m_sent{0};
};

RBSYS_SERVICE(NotificationService) // NOLINT
