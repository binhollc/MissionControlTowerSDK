#pragma once
#include <string>
#include <cstdio>

class BridgeReader {
public:
    BridgeReader(FILE* bridgeProcess);
    bool hasMoreData() const;
    std::string readNextData(bool nonBlocking = true);
private:
    FILE* bridgeProcess;
    std::string buffer;
};
