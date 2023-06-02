from command_bridge.base_command_adaptor import BaseCommandAdaptor, command
from command_bridge.adaptor_errors import Error
from mock_host_adapter_sdk import MockHostAdapterSDK

class MockCommandAdaptor(BaseCommandAdaptor):
    def __init__(self):
        super().__init__()
        self.test = "what"
        self.sdk = MockHostAdapterSDK()
        self.sdk.onCallback(self.callback)

    @command("open")
    def open(self, address=None):
        return Error(f"Device {address} not available.")

    @command("info", is_async=True)
    def info(self):
        return "info"

    @command("pwm")
    def gpio_pwm(self, pin=None, freq=None, dc=None):
        print(pin)
        print(freq)
        print(dc)
        print(self.transaction_id)
        return self.test

    @command("close")
    def close(self):
        return "close"

    def callback(self, sdk_response):
        if (sdk_response == "error"):
            self.respond(0, Error(sdk_response.error.message))
        else:
            self.respond(sdk_response['data'], sdk_response['transaction_id'])
