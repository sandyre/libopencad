#ifndef __DWG_FORMAT_SENTINELS_H__
#define __DWG_FORMAT_SENTINELS_H__

class DWG_SENTINELS
{
public:
    static const size_t SENTINEL_LENGTH = 16;

    static constexpr const char* HEADER_VARIABLES_START
        = "\xCF\x7B\x1F\x23\xFD\xDE\x38\xA9\x5F\x7C\x68\xB8\x4E\x6D\x33\x5F";
    static constexpr const char* HEADER_VARIABLES_END
        = "\x30\x84\xE0\xDC\x02\x21\xC7\x56\xA0\x83\x97\x47\xB1\x92\xCC\xA0";
};

#endif
