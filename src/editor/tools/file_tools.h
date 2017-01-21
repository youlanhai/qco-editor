#ifndef __Editor__FileTools__
#define __Editor__FileTools__

#include "../config.h"
#include <cstdlib>
#include <string>
#include <vector>

namespace Editor
{
	typedef std::vector<std::string> StringVector;

    EDITOR_DLL extern const std::string empty_string;

    EDITOR_DLL void formatSlash(std::string & path);
    EDITOR_DLL void formatPath(std::string & path);
    EDITOR_DLL void formatPathNoEndSlash(std::string & path);
    
    EDITOR_DLL void appendPathSlash(std::string & path);
    EDITOR_DLL void trimPathSlash(std::string & path);
    
    EDITOR_DLL std::string getFileExt(const std::string & filename);
    EDITOR_DLL void removeFileExt(std::string & name);

    EDITOR_DLL std::string getFilePath(const std::string & filename);
    EDITOR_DLL std::string getFileName(const std::string & filename);
    EDITOR_DLL std::string joinPath(const std::string & a, const std::string & b);
    EDITOR_DLL std::string getParentPath(const std::string & filename, int tier);
    
    EDITOR_DLL bool stringStartWith(const char *src, const char *prefix);
    EDITOR_DLL bool stringEndWith(const char *src, const char *postfix);
    
    EDITOR_DLL void normalizePath(std::string & path);
    
    EDITOR_DLL bool listDir(const std::string & path, StringVector & files);
    EDITOR_DLL bool isFile(const std::string & path);
    EDITOR_DLL bool isDir(const std::string & path);
    EDITOR_DLL bool isFileExist(const std::string & path);
    
    EDITOR_DLL std::string getExePath();
    EDITOR_DLL std::string getAppResPath();
    EDITOR_DLL std::string getAppModulePath();

    EDITOR_DLL std::string number2str(int v);
}

#endif /* defined(__Editor__FileTools__) */
