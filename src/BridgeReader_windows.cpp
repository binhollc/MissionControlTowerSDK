#include "BridgeReader_windows.h"
#include <string>
#include <vector>
#include <algorithm>

BridgeReader::BridgeReader(HANDLE bridgeProcessRead) : bridgeProcessRead(bridgeProcessRead) {}

std::string BridgeReader::readNextData() {
    if (bridgeProcessRead == NULL) {
        return "__EOF__";
    }

    const int bufferSize = 1024;
    DWORD bytesRead;
    char buffer[bufferSize];
    std::vector<char> line;

    while (true) {
        if (!ReadFile(bridgeProcessRead, buffer, bufferSize - 1, &bytesRead, NULL) || bytesRead == 0) {
            // EOF or error
            return "__EOF__";
        }

        buffer[bytesRead] = '\0'; // Null-terminate what we've read
        line.insert(line.end(), buffer, buffer + bytesRead);

        // Check if we have a full line
        auto it = std::find(line.begin(), line.end(), '\n');
        if (it != line.end()) {
            // Prepare result up to '\n'
            std::string result(line.begin(), it);

            // Save the rest for next time
            line = std::vector<char>(it + 1, line.end());

            return result;
        }
    }
}
