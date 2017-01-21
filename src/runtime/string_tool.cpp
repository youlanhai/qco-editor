#include "string_tool.h"

NS_PIP_BEGIN

const std::string empty_string;
    
void formatPath(std::string & path)
{
    if(!path.empty() && path.back() != '/')
    {
        path += '/';
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
    
    if(ret.back() == '/')
    {
        ret.pop_back();
    }
    
    std::string::size_type pos = ret.find_last_of('/');
    if(pos != std::string::npos)
    {
        return ret.erase(pos);
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
    
    if(ret.back() == '/')
    {
        ret.pop_back();
    }
    
    std::string::size_type pos = ret.find_last_of('/');
    if(pos != std::string::npos)
    {
        return ret.substr(pos+1);
    }
    
    return empty_string;
}

std::string joinPath(const std::string & a, const std::string & b)
{
    std::string ret = a;
    formatPath(ret);
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

NS_PIP_END
