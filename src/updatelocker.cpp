#include "KCL/updatelocker.h"

UpdateLocker::UpdateLocker() :
    updateCount_(0)
{
}

UpdateLocker::~UpdateLocker()
{
}

bool UpdateLocker::updating()
{
    return updateCount_;
}

void UpdateLocker::beginUpdate()
{
    if (!updating())
        startingUpdate();

    ++updateCount_;
}

void UpdateLocker::endUpdate()
{
    --updateCount_;

    if (!updating())
        finishUpdate();
}

void UpdateLocker::startingUpdate()
{
}

void UpdateLocker::finishUpdate()
{
}
