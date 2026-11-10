// Retention check driver. Calls the same function a normal run calls.
// The date is an argument so the check does not follow the wall clock.

#include "logger.h"

int main(int argc, char **argv)
{
    if (argc != 4) {
        return 2;
    }
    logger_maintain_directory(argv[1], argv[2], argv[3]);
    return 0;
}
