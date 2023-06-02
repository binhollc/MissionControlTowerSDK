#pragma once
#include <string>
#include <cstdio>

class BridgeReader {
public:
    BridgeReader(FILE* bridgeProcess);
    bool hasMoreData() const;
    std::string readNextData();
private:
    FILE* bridgeProcess;
    std::string buffer;
};
