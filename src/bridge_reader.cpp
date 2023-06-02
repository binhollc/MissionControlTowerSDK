#include "bridge_reader.h"
#include <iostream>

BridgeReader::BridgeReader(FILE* bridgeProcess) : bridgeProcess(bridgeProcess) {}

bool BridgeReader::hasMoreData() const {
    // TODO: Implement logic to check if more data is available from the bridge
}

std::string BridgeReader::readNextData() {
    std::cout << "[INI] std::string BridgeReader::readNextData()" << "\n";

    char tmpBuffer[128];
    while (fgets(tmpBuffer, sizeof(tmpBuffer), bridgeProcess) != nullptr) {
        std::cout << "Data read from bridge process" << "\n";

        buffer += tmpBuffer;

        std::size_t pos = buffer.find("\n");
        if (pos != std::string::npos) {
            std::string data = buffer.substr(0, pos);
            buffer = buffer.substr(pos + 1);
            return data;
        }
    }

    std::cout << "[END] std::string BridgeReader::readNextData()" << "\n";

    return "";
}
