#ifndef RUNTIME_PIP_CONFIG_H
#define RUNTIME_PIP_CONFIG_H

#define NS_PIP_BEGIN    namespace pip{
#define NS_PIP_END      }
#define USING_NS_PIP    using namespace pip

#ifdef BUILD_PIP_STATIC
#  define PIP_DLL
#else // BUILD_PIP_STATIC
#  ifdef _WIN32
#    ifdef BUILD_PIP_SHARED
#      define PIP_DLL 	   __declspec(dllexport)
#    else
#      define PIP_DLL      __declspec(dllimport)
#    endif // BUILD_PIP_SHARED
#  else
#     define PIP_DLL
#  endif // _WIN32
#endif // BUILD_PIP_STATIC

#endif /* RUNTIME_PIP_CONFIG_H */
