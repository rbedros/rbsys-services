# rbsys-services

Microservice plugins for the [rbsys](https://github.com/rbedros/rbsys) framework.

## Services

| Service | Port | Broker | Description |
|---------|------|--------|-------------|
| OrderService | 8081 | RabbitMQ | Receives orders, forwards to inventory |
| InventoryService | 8082 | RabbitMQ | Checks stock, reserves inventory |
| NotificationService | 8083 | RabbitMQ | Sends customer notifications |
| AnalyticsService | 8084 | Kafka | Real-time order event analytics |

## Architecture

```
OrderService --AMQP--> InventoryService --AMQP--> NotificationService
     |
     +--Kafka--> AnalyticsService
```

## Build

```bash
cmake -B build -G Ninja -DRBSYS_DIR=../rbsys
cmake --build build
```

## Deploy

```bash
# Sign plugins
python3 ../rbsys/tools/sign_plugin.py --key $SECRET build/services/order/order-service.so

# Run daemon with hot-reload
rbsys-daemon --config config/services.xml --plugins ./plugins/ --signing-key $SECRET --watch
```

## Creating a New Service

```cpp
#include <Service.h>

class MyService : public rbsys::Service {
    std::string name() const override { return "MyService"; }
    void onStart() override {
        subscribe("my.topic", [this](const Message& msg) {
            publish("output.topic", msg.payload);
        });
    }
};
RBSYS_SERVICE(MyService)
```
