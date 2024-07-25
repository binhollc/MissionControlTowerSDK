#include "BridgeReader_windows.h"
#include <string>
#include <vector>
#include <algorithm>

BridgeReader::BridgeReader(HANDLE hPipeOutputRead) : hPipeOutputRead(hPipeOutputRead) {}

std::string BridgeReader::readNextData(bool nonBlocking) {
    if (hPipeOutputRead == NULL) {
        return "__EOF__";
    }

    const int bufferSize = 1024;
    DWORD bytesRead;
    char buffer[bufferSize];
    std::vector<char> line;

    if (nonBlocking) {
        DWORD bytesAvailable;
        if (!PeekNamedPipe(hPipeOutputRead, NULL, 0, NULL, &bytesAvailable, NULL) || bytesAvailable == 0) {
            return "";
        }
    }

    while (true) {
        if (!ReadFile(hPipeOutputRead, buffer, bufferSize - 1, &bytesRead, NULL) || bytesRead == 0) {
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
