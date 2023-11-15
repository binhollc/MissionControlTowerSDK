#include "sample_library.h"
#include "CommandDispatcher.h"

void SampleLibrary::get_address() {
    CommandDispatcher dispatcher("ListUsbDevices");

    dispatcher.start();

    dispatcher.invokeCommandSync("0", "list", {
        {"serialFilter", {{1240, 60724}}},
        {"hidFilter", {{1240, 221}, {8137, 33532}}}
    }, [this](CommandResponse cr) {
        if (cr.status == "success") {
          for (auto& device : cr.data) {
            if (device["product"].template get<std::string>() == "Binho Supernova") {
              device_address = device["product"].template get<std::string>();
              break;
            }
          }
        }
    });

    dispatcher.waitForAllCommands();

    dispatcher.invokeCommand("0", "exit", {});

    dispatcher.stop();
}

void SampleLibrary::load_values() {
    if (device_address.empty()) {
      throw std::runtime_error("No device address specified");
    }

    CommandDispatcher dispatcher("BinhoSupernova");

    dispatcher.start();

    dispatcher.invokeCommandSync("0", "open", {{"address", device_address}}, [this](CommandResponse cr) {
      open_ok = true;
    });

    if (!open_ok) {
      throw std::runtime_error("Failed to open the target device");
    }

    dispatcher.invokeCommandSync("0", "get_usb_string", {{"subCommand", "HW_VERSION"}}, [this](CommandResponse cr) {
      hw_version = cr.data["data"];
    });

    dispatcher.invokeCommandSync("0", "get_usb_string", {{"subCommand", "FW_VERSION"}}, [this](CommandResponse cr) {
      fw_version = cr.data["data"];
    });

    dispatcher.invokeCommandSync("0", "get_usb_string", {{"subCommand", "SERIAL_NUMBER"}}, [this](CommandResponse cr) {
      serial_number = cr.data["data"];
    });

    dispatcher.waitForAllCommands();
    dispatcher.invokeCommand("0", "exit", {});
    dispatcher.stop();
}
