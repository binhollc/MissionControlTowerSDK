#include "CommandDispatcher.h"
#include <iostream>

int main() {
  CommandDispatcher dispatcher("BinhoNova");

  dispatcher.start();

  dispatcher.invokeCommandSync("1", "open", {{"address", "NovaSimulatedPort"}}, [](CommandResponse cr) {
      std::cout << cr.transaction_id << cr.status << cr.is_promise << cr.data.dump() << "\n";
  });

  for (int j = 1; j <= 7; ++j) {
      // Forward loop
      for (int i = 1; i <= 10; ++i) {
          int dc = i * 10;  // Duty cycle goes from 10 to 100 in increments of 10
          dispatcher.invokeCommandSync(std::to_string(i), "gpio", {
              {"channel", 0},
              {"type", "pwm"},
              {"params", {
                  {"dc", std::to_string(dc)}
              }}
          }, [](CommandResponse cr) {
              std::cout << cr.transaction_id << cr.status << cr.is_promise << cr.data.dump() << "\n";
          });
          // std::this_thread::sleep_for(std::chrono::milliseconds(50));
      }

      // Backward loop
      for (int i = 10; i >= 1; --i) {
          int dc = i * 10;  // Duty cycle goes from 100 to 10 in increments of -10
          dispatcher.invokeCommandSync(std::to_string(i), "gpio", {
              {"channel", 0},
              {"type", "pwm"},
              {"params", {
                  {"dc", std::to_string(dc)}
              }}
          }, [](CommandResponse cr) {
              std::cout << cr.transaction_id << cr.status << cr.is_promise << cr.data.dump() << "\n";
          });
      }
  }

  dispatcher.waitForAllCommands();

  dispatcher.invokeCommand("0", "exit", {});

  dispatcher.stop();

  return 0;
}