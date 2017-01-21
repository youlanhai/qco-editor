#include "file_tools.h"
#include <platform/CCFileUtils.h>
#include <sstream>

#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#endif

#include <QApplication>


USING_NS_CC;

namespace Editor
{
    const std::string empty_string;

    void formatSlash(std::string & path)
    {
        std::replace(path.begin(), path.end(), '\\', '/');
    }
    
    void formatPath(std::string & path)
    {
        formatSlash(path);
        appendPathSlash(path);
    }
    
    void formatPathNoEndSlash(std::string & path)
    {
        formatSlash(path);
        trimPathSlash(path);
    }
    
    void appendPathSlash(std::string & path)
    {
        if(!path.empty() && path[path.size() - 1] != '/')
        {
            path += '/';
        }
    }
    
    void trimPathSlash(std::string & path)
    {
        if(!path.empty() && path[path.size() - 1] == '/')
        {
            path.erase(path.end() - 1);
        }
    }
    
    std::string getFileExt(const std::string & filename)
    {
        std::string::size_type pos = filename.find_last_of('.');
        if(pos != std::string::npos)
        {
            return filename.substr(pos);
        }
        
        return empty_string;
    }
    
    void removeFileExt(std::string & name)
    {
        std::string::size_type pos = name.find_last_of('.');
        if(pos != std::string::npos)
        {
            name.erase(pos);
        }
    }
    
    std::string getFilePath(const std::string & filename)
    {
        std::string ret = filename;
        if(ret.empty())
        {
            return ret;
        }
        
        trimPathSlash(ret);
        
        std::string::size_type pos = ret.find_last_of('/');
        if(pos != std::string::npos)
        {
            return ret.substr(0, pos);
        }
        
        return empty_string;
    }
    
    std::string getFileName(const std::string & filename)
    {
        std::string ret = filename;
        if(ret.empty())
        {
            return ret;
        }
        
        trimPathSlash(ret);
        
        std::string::size_type pos = ret.find_last_of('/');
        if(pos != std::string::npos)
        {
            return ret.substr(pos+1);
        }
        
        return ret;
    }
    
    std::string joinPath(const std::string & a, const std::string & b)
    {
        if(a.empty())
        {
            return b;
        }
        
        if(b.empty())
        {
            return a;
        }
        
        std::string ret = a;
        formatPath(ret);
        if(b[0] == '/')
        {
            ret.erase(ret.end() - 1);
        }
        ret += b;
        return ret;
    }
    
    std::string getParentPath(const std::string & filename, int tier)
    {
        if(tier < 1 || filename.empty())
        {
            return filename;
        }
        
        int i = int(filename.size()) - 1;
        
        // ignore the last slash.
        if(filename[i] == '/' || filename[i] == '\\')
        {
            --i;
        }
        
        for(; i >= 0 && tier > 0; --i)
        {
            if(filename[i] == '/' ||
               filename[i] == '\\')
            {
                --tier;
            }
        }
        
        ++i;
        return filename.substr(0, i);
    }
    
    bool stringStartWith(const char *src, const char *prefix)
    {
        while(*src != 0 && *prefix !=0 && *src == *prefix)
        {
            ++src;
            ++prefix;
        }
        
        return *prefix == 0;
    }
    
    bool stringEndWith(const char *src, const char *postfix)
    {
        size_t srcLen = strlen(src);
        size_t dstLen = strlen(postfix);
        
        if(srcLen >= dstLen)
        {
            return stringStartWith(src + srcLen - dstLen, postfix);
        }
        return false;
    }

    bool listDir(const std::string & path, StringVector & files)
    {
        if(path.empty())
        {
            return false;
        }
        
        std::string filePath = path;
        if(filePath[filePath.size() - 1] == '/')
        {
            filePath.erase(filePath.size() - 1);
        }
        
        filePath = FileUtils::getInstance()->fullPathForFilename(filePath);
        if(filePath.empty())
        {
            return false;
        }
        
        StringVector onlyFiles;
        
    #if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
        
        DIR *dp;
        struct dirent *entry;
        struct stat statbuf;
        if((dp = opendir(filePath.c_str())) == NULL)
        {
            return 0;
        }
        
        if(filePath[filePath.size() - 1] != '/')
        {
            filePath += '/';
        }
        
        while((entry = readdir(dp)) != NULL)
        {
            if(strcmp(entry->d_name, ".") == 0 ||
               strcmp(entry->d_name, "..") == 0)
            {
                continue;
            }
            
            std::string fname(entry->d_name, entry->d_namlen);
            
            std::string path = filePath + fname;
            lstat(path.c_str(), &statbuf);
            if(S_ISDIR(statbuf.st_mode))
            {
                fname += '/';
                files.push_back(fname);
            }
            else if(S_ISREG(statbuf.st_mode))
            {
                onlyFiles.push_back(fname);
            }
        }
        closedir(dp);
        
        files.insert(files.end(), onlyFiles.begin(), onlyFiles.end());
        return true;
    #endif
        return false;
    }

    bool isFile(const std::string & path)
    {
        std::string filePath = FileUtils::getInstance()->fullPathForFilename(path);
        if(filePath.empty())
        {
            return false;
        }
        
        bool ret = false;
    #if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
        struct stat statbuf;
        if(0 == lstat(filePath.c_str(), &statbuf))
        {
            ret = (S_ISREG(statbuf.st_mode));
        }
    #endif
        return ret;
    }

    bool isDir(const std::string & path)
    {
        std::string filePath = FileUtils::getInstance()->fullPathForFilename(path);
        if(filePath.empty())
        {
            return false;
        }
        if(filePath[filePath.size() - 1] == '/')
        {
            filePath.erase(filePath.end() - 1);
        }
        
        bool ret = false;
        
    #if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
        struct stat statbuf;
        if(0 == lstat(filePath.c_str(), &statbuf))
        {
            ret = (S_ISDIR(statbuf.st_mode));
        }
    #endif
        
        return ret;
    }

    bool isFileExist(const std::string & path)
    {
        std::string filePath = FileUtils::getInstance()->fullPathForFilename(path);
        if(filePath.empty())
        {
            return 0;
        }
        bool ret = false;
        
    #if CC_TARGET_PLATFORM == CC_PLATFORM_MAC
        ret = (access(filePath.c_str(), F_OK) == 0);
    #endif
        
        return ret;
    }

    
    void normalizePath(std::string & path)
    {
        if(path.empty())
        {
            return;
        }
        
        formatSlash(path);
        
        std::string ret;
        ret.reserve(path.size());
        
        std::vector<size_t> dirs;
        char ch;
        bool startWithSlash = path[0] != '/';
        for(size_t i = 0; i < path.size(); ++i)
        {
            ch = path[i];
            
            if(ch == '/')
            {
                if(!startWithSlash) // may be "//" or "/./"
                {
                    ret += '/';
                    dirs.push_back(i);
                }
                startWithSlash = true;
                continue;
            }
            
            if(startWithSlash && ch == '.')
            {
                if(i + 1 >= path.size()) // '/.'
                {
                    continue;
                }
                else if(path[i + 1] == '/') // '/./'
                {
                    continue;
                }
                else if(path[i + 1] == '.') // '/..'
                {
                    if(i + 2 >= path.size() || path[i + 2] == '/') 
                    {
                        //this is a valid path '..'
                        //so we need jump to parent path.
                        if(dirs.size() > 1)
                        {
                            dirs.pop_back();
                        }
                        
                        if(dirs.size() > 1)
                        {
                            ret.erase(dirs.back() + 1);
                        }
                        i += 2;
                        continue;
                    }
                }
            }
            
            ret += ch;
            startWithSlash = false;
        }
        
        path = ret;
    }

    std::string number2str(int v)
    {
        std::ostringstream ss;
        ss << v;
        return ss.str();
    }

    std::string getExePath()
    {
        return qApp->applicationDirPath().toStdString();
    }

    std::string getAppResPath()
    {
#ifdef Q_OS_MAC
        std::string path = getExePath();
        return joinPath(getFilePath(path), "Resources");
#else
        return getAppModulePath();
#endif
    }

    std::string getAppModulePath()
    {
#ifdef Q_OS_MAC
        // retpath/xxx.app/Contents/MacOS
        return getParentPath(getExePath(), 3);
#else
        return getExePath();
#endif
    }

}// end namespace Editor
