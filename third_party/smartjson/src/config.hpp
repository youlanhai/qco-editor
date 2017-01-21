//
//  config.hpp
//  smartjson
//
//  Created by youlanhai on 15/12/19.
//  Copyright © 2015年 youlanhai. All rights reserved.
//

#ifndef json_config_h
#define json_config_h

#ifdef BUILD_SMARTJSON_STATIC
#	define SMARTJSON_DLL
#else

#ifdef _WIN32
#ifdef BUILD_SMARTJSON_SHARED
#	define SMARTJSON_DLL	__declspec(dllexport)
#else
#	define SMARTJSON_DLL 	__declspec(dllimport)
#endif
#else
#	define SMARTJSON_DLL
#endif

#endif


#define JSON_USE_LARGE_NUMBER 1

#define JSON_CODE_INLINE 1

#define JSON_SUPPORT_STL_STRING 1

#if JSON_CODE_INLINE
#   define JSON_INLINE inline
#else
#   define JSON_INLINE
#endif

#ifndef JSON_ASSERT
#   ifdef DEBUG
#       define JSON_ASSERT(EXP) assert(EXP)
#   else
#       define JSON_ASSERT(EXP)
#   endif
#endif //JSON_ASSERT

#endif /* json_config_h */
