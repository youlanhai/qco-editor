#ifndef __Editor__LogEventListener__
#define __Editor__LogEventListener__

#include "../config.h"
#include "../event_listener.h"
#include "runtime/singleton.h"

#include <vector>
#include <functional>
#include <string>

#if LOG_THEAD_SAFE
#include <pthread.h>
#endif

namespace Editor
{

typedef std::function<void(int priority, const char * tag, const char * log)> LogEventListener;

class EDITOR_DLL LogEventDispatcher : public Singleton<LogEventDispatcher>
{
public:
    LogEventDispatcher();
    ~LogEventDispatcher();

    // default is disabled.
    void setLogFile(const std::string &logFile);
    void setLogFilePriority(int priority);
    
    void dispatchLog(int priority, const char * tag, const char * log);
    
    void subscribe(const std::string &key, LogEventListener listener);
    void unsubscribe(const std::string &key);
    
    void lock();
    void unlock();
    
private:
    typedef std::vector<std::pair<std::string, LogEventListener> > Listeners;
    Listeners       listener_;
    
#if LOG_THEAD_SAFE
    pthread_mutex_t mutex_;
#endif

    FILE*           file_;
    std::string     logFilePath_;
    int             logFilePriority_;
};

}

#endif /* defined(__Editor__LogEventListener__) */
