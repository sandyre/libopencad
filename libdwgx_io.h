#ifndef __LIBDWGX_DWG_IO_H__
#define __LIBDWGX_DWG_IO_H__

#include <string>

long long   ReadRAWLONGLONG ( const char * input_array, size_t& bitOffsetFromStart );
int         ReadRAWLONG ( const char * input_array, size_t& bitOffsetFromStart );
short       ReadRAWSHORT ( const char * input_array, size_t& bitOffsetFromStart );
char        Read3B ( const char * input_array, size_t& bitOffsetFromStart );

bool        ReadBIT ( const char * input_array, size_t& bitOffsetFromStart );
short       ReadBITSHORT ( const char * input_array, size_t& bitOffsetFromStart );
char        ReadCHAR ( const char * input_array, size_t& bitOffsetFromStart );
double      ReadBITDOUBLE ( const char * input_array, size_t& bitOffsetFromStart );
long long   ReadMCHAR ( const char * input_array, size_t& bitOffsetFromStart );
int         ReadMSHORT ( const char * input_array, size_t& bitOffsetFromStart );
std::string ReadTV ( const char * input_array, size_t& bitOffsetFromStart );

#endif
