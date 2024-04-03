#include <gtest/gtest.h>
#include "CommandDispatcher.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>

// Global constant for minimum tolerable waiting time for a command
constexpr std::chrono::milliseconds MIN_TOLERABLE_CMD_WAIT_MS(300);

class SupernovaGenericCommandFlowTest : public ::testing::Test {
protected:
    CommandDispatcher dispatcher;

    SupernovaGenericCommandFlowTest() : dispatcher("BinhoSupernova") {}

    void SetUp() override {
      dispatcher.start();
    }

    void TearDown() override {
      dispatcher.waitForAllCommands();
      dispatcher.stop();
    }
};

TEST_F(SupernovaGenericCommandFlowTest, OpenCloseExit_SimulatedPort_OperationsCompleteSuccessfully) {
    bool openInvoked = false;
    bool closeInvoked = false;
    bool exitInvoked = false;

    // Open the SupernovaSimulatedPort
    dispatcher.invokeCommand("1", "open", {{"address", "SupernovaSimulatedPort"}}, [&](CommandResponse cr) {
      openInvoked = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(MIN_TOLERABLE_CMD_WAIT_MS));

    EXPECT_TRUE(openInvoked);

    dispatcher.waitFor({"1"});

    // TODO: Uncomment the code below when https://focusuy.atlassian.net/browse/BMC2-991 is solved:

    // dispatcher.invokeCommand("2", "close", {}, [&](CommandResponse cr) {
    //   closeInvoked = true;
    // });

    // std::this_thread::sleep_for(std::chrono::milliseconds(MIN_TOLERABLE_CMD_WAIT_MS));

    // EXPECT_TRUE(closeInvoked);

    dispatcher.invokeCommand("1000", "exit", {}, [&](CommandResponse cr) {
      exitInvoked = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(MIN_TOLERABLE_CMD_WAIT_MS));

    EXPECT_TRUE(exitInvoked);

    dispatcher.waitFor({"1000"});
}
