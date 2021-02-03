#include <iostream>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#else
#include <windows.h>
#endif
#include "can_api.h"

#ifndef CHANNEL
#define CHANNEL  0x52
#endif
#ifndef BAUDRATE
#define BAUDRATE  CANBTR_INDEX_250K
#endif
#define FRAMES  (CAN_MAX_STD_ID+1)

int main(int argc, const char * argv[]) {
    int handle, result, i;
    can_bitrate_t bitrate;
    can_message_t message;

    std::cout << can_version() << std::endl;
    if ((handle = can_init(CHANNEL, CANMODE_DEFAULT, NULL)) < 0) {
        std::cerr << "+++ error: interface could not be initialized" << std::endl;
        return -1;
    }
    bitrate.index = BAUDRATE;
    if ((result = can_start(handle, &bitrate)) < 0) {
        std::cerr << "+++ error: interface could not be started" << std::endl;
        goto end;
    }
    std::cout << ">>> Be patient..." << std::flush;
    message.xtd = message.rtr = message.sts = 0;
    for (i = 0; i < FRAMES; i++) {
        message.id = (uint32_t)i & CAN_MAX_STD_ID;
        message.dlc = 8U;
        message.data[0] = (uint8_t)((uint64_t)i >> 0);
        message.data[1] = (uint8_t)((uint64_t)i >> 8);
        message.data[2] = (uint8_t)((uint64_t)i >> 16);
        message.data[3] = (uint8_t)((uint64_t)i >> 24);
        message.data[4] = (uint8_t)((uint64_t)i >> 32);
        message.data[5] = (uint8_t)((uint64_t)i >> 40);
        message.data[6] = (uint8_t)((uint64_t)i >> 48);
        message.data[7] = (uint8_t)((uint64_t)i >> 56);
        if ((result = can_write(handle, &message, 0U)) < 0) {
            std::cerr << "+++ error: interface could not be stopped" << std::endl;
            goto reset;;
        }
    }
#if !defined(_WIN32) && !defined(_WIN64)
    usleep(1000000);  // afterburner
#else
    Sleep(1000);    // wait a minute
#endif
reset:
    std::cout << i << " frame(s) sent" << std::endl;
    if ((result = can_reset(handle)) < 0)
        std::cerr << "+++ error: interface could not be stopped" << std::endl;
end:
    if ((result = can_exit(handle)) < 0)
        std::cerr << "+++ error: interface could not be shutdown" << std::endl;
    return result;
}
