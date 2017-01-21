#ifndef coc_stream_h
#define coc_stream_h

#include "coc_config.h"

namespace pip
{
    class ReaderSameEndian;
    class WriterSameEndian;
}

NS_COC_BEGIN

typedef pip::ReaderSameEndian BinaryReader;
typedef pip::WriterSameEndian BinaryWriter;

NS_COC_END


#endif /* coc_stream_h */
