import threading
import time

class MockHostAdapterSDK:
    def __init__(self):
        self.callback = None
        self.callback_thread = threading.Thread(target=self._run_callback)
        self.callback_thread.start()

    def onCallback(self, callback):
        self.callback = callback

    def _run_callback(self):
        while True:
            time.sleep(5)
            if self.callback is not None:
                sdk_response = {"transaction_id": 123, "data": [0xDE,0xAD,0xBE,0xEF]}
                self.callback(sdk_response)

    def stop(self):
        self.callback_thread.join()
