#pragma once
#include <string>
#include <cstdio>

class BridgeReader {
public:
    explicit BridgeReader(int pipeFd);
    bool hasMoreData() const;
    std::string readNextData(bool nonBlocking = true);
private:
    int pipeFd;
    std::string buffer;
};
