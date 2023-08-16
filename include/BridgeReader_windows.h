#pragma once

#include <windows.h>
#include <vector>
#include <string>
#include <cstdio>

class BridgeReader {
public:
    BridgeReader(HANDLE bridgeProcessRead);
    std::string readNextData();
private:
    HANDLE bridgeProcessRead;
    std::vector<char> line;
};
