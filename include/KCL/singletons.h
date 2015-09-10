#ifndef SINGLETONS_H
#define SINGLETONS_H

#include "kcl_global.h"

#include <QList>
#include <QGlobalStatic>

typedef void * (*SingletonInitializerFunc)();

class KCL_EXPORT SingletonRegistry
{
public:
    static SingletonRegistry &instance();
    #define Singletons SingletonRegistry::instance()

    void registerInitializer(SingletonInitializerFunc myFunction);

    void initialize();

private:
    QList<SingletonInitializerFunc> initializers_;
};

#define KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY(REGISTRY, TYPE, NAME, ARGS) \
    Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS) \
    struct SingletonInitializer_ ##NAME \
    { \
        SingletonInitializer_ ##NAME() { REGISTRY->registerInitializer(&SingletonInitializer_ ##NAME::initializer); } \
        static void *initializer() { return NAME(); } \
    }; \
    static SingletonInitializer_ ##NAME singletonInitializer_ ##NAME;

#define KCL_SINGLETON_REGISTER_AT_REGISTRY(REGISTRY, TYPE, NAME) KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY(REGISTRY, TYPE, NAME, ())

#define KCL_SINGLETON_WITH_ARGS_REGISTER(TYPE, NAME, ARGS) KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY((&SingletonRegistry::instance()), TYPE, NAME, ARGS)
#define KCL_SINGLETON_REGISTER(TYPE, NAME) KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY((&SingletonRegistry::instance()), TYPE, NAME, ())

#endif // SINGLETONS_H
