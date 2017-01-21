#ifndef RUNTIME_STRING_TOOL_H
#define RUNTIME_STRING_TOOL_H

#include "pip_config.h"
#include <string>

NS_PIP_BEGIN

// extern from helper.cpp
PIP_DLL extern const std::string empty_string;

PIP_DLL void formatPath(std::string &path);

/**
 *  获取文件扩展名。返回值包含'.'。
 */
PIP_DLL std::string getFileExt(const std::string &filename);
PIP_DLL void removeFileExt(std::string &name);

PIP_DLL std::string getFilePath(const std::string &filename);
PIP_DLL std::string getFileName(const std::string &filename);
PIP_DLL std::string joinPath(const std::string &a, const std::string &b);
PIP_DLL std::string getParentPath(const std::string &filename, int tier);
    
NS_PIP_END

#endif // RUNTIME_STRING_TOOL_H
