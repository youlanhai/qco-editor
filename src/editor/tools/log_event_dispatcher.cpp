#include "log_event_dispatcher.h"
#include "time_tool.h"
#include "log_tool.h"

IMPLEMENT_SINGLETON(Editor::LogEventDispatcher);

namespace Editor
{

LogEventDispatcher::LogEventDispatcher()
    : file_(nullptr)
    , logFilePriority_(LOG_PRIORITY_DEBUG)
{
#if LOG_THEAD_SAFE
    pthread_mutex_init(&mutex_, NULL);
#endif
}

LogEventDispatcher::~LogEventDispatcher()
{
    lock();
    if(file_ != nullptr)
    {
        fputs("Log file system stopped.\n", file_);
        fclose(file_);
        file_ = nullptr;
    }
    unlock();

#if LOG_THEAD_SAFE
    pthread_mutex_destroy(&mutex_);
#endif
}

void LogEventDispatcher::lock()
{
#if LOG_THEAD_SAFE
    pthread_mutex_lock(&mutex_);
#endif
}

void LogEventDispatcher::unlock()
{
#if LOG_THEAD_SAFE
    pthread_mutex_unlock(&mutex_);
#endif
}

void LogEventDispatcher::dispatchLog(int priority, const char * tag, const char * log)
{
    lock();

    if(file_ != nullptr && priority >= logFilePriority_)
    {
        struct tm timeinfo;
        getLocalTime(timeinfo);

        const char * prefix = getLogPriorityString(priority);

        fprintf(file_, "[%2.2d:%2.2d:%2.2d][%s][%s]", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec, prefix, tag);
        fputs(log, file_);
        fputs("\n", file_);
        fflush(file_);
    }
    
    for(Listeners::iterator it = listener_.begin();
        it != listener_.end(); ++it)
    {
        it->second(priority, tag, log);
    }
    
    unlock();
}

void LogEventDispatcher::subscribe(const std::string &key, LogEventListener listener)
{
    lock();
    for(Listeners::iterator it = listener_.begin(); it != listener_.end(); ++it)
    {
        if(it->first == key)
        {
            listener_.erase(it);
            break;
        }
    }
    listener_.push_back(std::make_pair(key, listener));
    unlock();
}

void LogEventDispatcher::unsubscribe(const std::string &key)
{
    lock();
    for(Listeners::iterator it = listener_.begin(); it != listener_.end(); ++it)
    {
        if(it->first == key)
        {
            listener_.erase(it);
            break;
        }
    }
    unlock();
}

void LogEventDispatcher::setLogFile(const std::string &logFile)
{
    if(logFilePath_ == logFile)
    {
        return;
    }

    lock();

    logFilePath_ = logFile;
    if(file_ != nullptr)
    {
        fclose(file_);
        file_ = nullptr;
    }

    if(!logFilePath_.empty())
    {
        file_ = fopen(logFilePath_.c_str(), "w");
        if(file_ != nullptr)
        {
            fputs("Log file system start...\n", file_);
            fflush(file_);
        }
    }

    unlock();
}

void LogEventDispatcher::setLogFilePriority(int priority)
{
    logFilePriority_ = priority;
}

} //namespace Editor
