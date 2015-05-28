#include "../include/KCL/paranoidretrier.h"

#include <unistd.h>
#include <functional>

#include "KCL/sleepaid.h"

bool ParanoidRetrier::stopRetries(bool success, int &tries, int maxTries, int waitMs)
{
    ++tries;

    if (!success && waitMs > 0)
        SleepAid::msleep(waitMs);

    return success || tries >= maxTries;
}

bool ParanoidRetrier::retry(std::function<bool()> func, int maxTries, int waitMs)
{
    int tries = 0;
    bool success = false;

    do
    {
        success = func();
    }
    while (!stopRetries(success, tries, maxTries, waitMs));

    return success;
}
