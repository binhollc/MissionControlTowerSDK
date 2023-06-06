#include "bridge_reader.h"
#include <iostream>
#include <fcntl.h>
#include <sys/select.h>
#include <unistd.h>

BridgeReader::BridgeReader(FILE* bridgeProcess) : bridgeProcess(bridgeProcess) {}

bool BridgeReader::hasMoreData() const {
    fd_set set;
    struct timeval timeout;

    FD_ZERO(&set); /* clear the set */
    FD_SET(fileno(bridgeProcess), &set); /* add our file descriptor to the set */

    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    return select(FD_SETSIZE, &set, NULL, NULL, &timeout) == 1;
}

std::string BridgeReader::readNextData() {
    char tmpBuffer[128];
    int fd = fileno(bridgeProcess);
    fcntl(fd, F_SETFL, O_NONBLOCK);  // Make reading non-blocking

    ssize_t result = read(fd, tmpBuffer, sizeof(tmpBuffer) - 1);

    if (result < 0) {
        // If errno == EAGAIN, that means we have read all data.
        // So go back to the main loop.
        if (errno == EAGAIN) {
            // std::cout << "read all data" << "\n";
            return "";
        } else {
            // Error occurred, report it and exit.
            perror("read");
            exit(EXIT_FAILURE);
        }
    } else if (result == 0) {
        // End of file. The remote has closed the connection.
        std::cout << "End of file" << "\n";
        return "";
    } else {
        // Ensure null-terminated string
        tmpBuffer[result] = '\0';

        std::cout << "Data read from bridge process" << "\n";

        buffer += tmpBuffer;

        std::size_t pos = buffer.find("\n");
        if (pos != std::string::npos) {
            std::string data = buffer.substr(0, pos);
            buffer = buffer.substr(pos + 1);
            return data;
        } else {
            return "";
        }
    }
}
