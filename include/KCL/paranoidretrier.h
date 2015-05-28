#ifndef PARANOIDRETRIER_H
#define PARANOIDRETRIER_H

#include <functional>

class ParanoidRetrier
{
public:
    static bool retry(std::function<bool()> func, int maxTries = 5, int waitMs = 0);

private:
    static bool stopRetries(bool success, int &tries, int maxTries, int waitMs);
};

#endif // PARANOIDRETRIER_H
