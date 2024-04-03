#include <gtest/gtest.h>
#include "CommandDispatcher.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>

// Global constant for minimum tolerable waiting time for a command
constexpr std::chrono::milliseconds MIN_TOLERABLE_CMD_WAIT_MS(2000);

class NotificationFlowTest : public ::testing::Test {
protected:
    CommandDispatcher dispatcher;

    NotificationFlowTest() : dispatcher("Mock") {}

    void SetUp() override {
      dispatcher.start();
    }

    void TearDown() override {
      dispatcher.waitForAllCommands();
      dispatcher.stop();
    }
};

TEST_F(NotificationFlowTest, NotificationReceived) {
  bool notificationReceived = false;

  dispatcher.onNotification([&](const CommandResponse& cr) {
      if (cr.transaction_id == "0") {
          notificationReceived = true;
      }
  });

  // Wait a sufficient amount of time for any asynchronous operations to complete.
  std::this_thread::sleep_for(MIN_TOLERABLE_CMD_WAIT_MS);

  // Assert that the notification was received.
  EXPECT_TRUE(notificationReceived);

  dispatcher.invokeCommandSync("1000", "exit", {});
};