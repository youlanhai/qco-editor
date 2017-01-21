#ifndef client_Singleton_h
#define client_Singleton_h

#include <cassert>

template<typename T>
class Singleton
{
public:
    Singleton()
    {
        assert(s_instance == nullptr);
    }

    ~Singleton()
    {

    }

    static void initInstance()
    {
        assert(s_instance == nullptr);
        s_instance = new T();
    }

    static void finiInstance()
    {
        if(s_instance != nullptr)
        {
            delete s_instance;
            s_instance = nullptr;
        }
    }

    static T * instance()
    {
        assert(s_instance != nullptr);
        return s_instance;
    }

    static bool hasInstance()
    {
        return s_instance != nullptr;
    }

private:
    static T * s_instance;
};

#define IMPLEMENT_SINGLETON(CLASS) \
    template<> CLASS * Singleton<CLASS>::s_instance = nullptr

#endif
