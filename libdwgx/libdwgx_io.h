#ifndef __LIBDWGX_DWG_IO_H__
#define __LIBDWGX_DWG_IO_H__

#include <string>

#include "libdwgx_datatypes.h"

int64_t     ReadRAWLONGLONG ( const char * input_array, size_t& bitOffsetFromStart );
int32_t     ReadRAWLONG ( const char * input_array, size_t& bitOffsetFromStart );
int16_t     ReadRAWSHORT ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadRAWDOUBLE ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     Read2B ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     Read3B ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     Read4B ( const char * input_array, size_t& bitOffsetFromStart );
DWG_HANDLE  ReadHANDLE ( const char * input_array, size_t& bitOffsetFromStart );

bool        ReadBIT ( const char * input_array, size_t& bitOffsetFromStart );
uint8_t     ReadCHAR ( const char * input_array, size_t& bitOffsetFromStart );
int16_t     ReadBITSHORT ( const char * input_array, size_t& bitOffsetFromStart );
int32_t     ReadBITLONG ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadBITDOUBLE ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadBITDOUBLEWD ( const char * input_array, size_t& bitOffsetFromStart, double defaultvalue );
int64_t     ReadMCHAR ( const char * input_array, size_t& bitOffsetFromStart );
uint32_t    ReadMSHORT ( const char * input_array, size_t& bitOffsetFromStart );
std::string ReadTV ( const char * input_array, size_t& bitOffsetFromStart );

#endif
