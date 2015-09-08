#include "KCL/singletons.h"

#include "KCL/debug.h"

Q_GLOBAL_STATIC(SingletonRegistry, singletonsInstance)

SingletonRegistry &SingletonRegistry::instance()
{
    return *singletonsInstance();
}

void SingletonRegistry::registerInitializer(SingletonInitializerFunc myFunction)
{
    initializers_.append(myFunction);
}

void SingletonRegistry::initialize()
{
    DGUARDMETHODTIMED;
    foreach (SingletonInitializerFunc initializer, initializers_)
        initializer();
}
