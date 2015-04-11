#ifndef UPDATELOCKER_H
#define UPDATELOCKER_H

#include "KCL/kcl_global.h"

class KCL_EXPORT UpdateLocker
{
public:
    UpdateLocker();
    virtual ~UpdateLocker();

    virtual void beginUpdate();
    virtual void endUpdate();

    virtual bool updating();

protected:
    int updateCount_;

    virtual void startingUpdate();
    virtual void finishUpdate();
};

#endif // UPDATELOCKER_H
