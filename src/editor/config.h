#ifndef Editor_EditorConfig_h
#define Editor_EditorConfig_h

#include <platform/CCPlatformConfig.h>

#ifdef BUILD_EDITOR_STATIC
#	define EDITOR_DLL
#else

#include <QtGlobal>
#ifdef BUILD_EDITOR_SHARED
#	define EDITOR_DLL Q_DECL_EXPORT
#else
#	define EDITOR_DLL Q_DECL_IMPORT
#endif

#endif

/** set value to 1, when we use log in multi thread. */
#define LOG_THEAD_SAFE 0

#endif
