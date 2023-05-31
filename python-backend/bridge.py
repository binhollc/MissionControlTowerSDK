from mock_command_adaptor import MockCommandAdaptor
from command_bridge.command_processor import CommandProcessor

adaptor = MockCommandAdaptor()
processor = CommandProcessor()

processor.register_adaptor(adaptor)

adaptor.start()
processor.start()
