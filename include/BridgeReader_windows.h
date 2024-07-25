#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <cstdio>

class BridgeReader {
public:
    BridgeReader(HANDLE hPipeOutputRead);
    std::string readNextData(bool nonBlocking = true);
private:
    HANDLE hPipeOutputRead;
    std::vector<char> line;
};
