#include "dwg_r2000.h"

int DWGFileR2000::ReadHeader()
{
    char * pabyBuf = new char[100];
    int32_t dImageSeeker, dSLRecords;
    int16_t dCodePage;

    fDWG.seekg ( 6, std::ios_base::seek_dir::beg ); // file version skipped.
    fDWG.seekg ( 7, std::ios_base::seek_dir::cur ); // meaningless data skipped.
    fDWG.read ( (char *) &dImageSeeker, 4 );

#ifdef _LIBDWGX_DEBUG
    printf ( "Image seeker readed: %d\n", dImageSeeker );
#endif

    fDWG.seekg ( 2, std::ios_base::seek_dir::cur );
    fDWG.read ( (char *) &dCodePage, 2 );

#ifdef _LIBDWGX_DEBUG
    printf ( "DWG Code page: %d\n",dCodePage );
#endif

    fDWG.read ( (char *) &dSLRecords, 4 );

#ifdef _LIBDWGX_DEBUG
    printf ( "Section-locator records count: %d\n", dSLRecords );
#endif

    this->fileHeader.paSLRecords = new SLRecord [ dSLRecords ];
    for ( size_t i = 0; i < dSLRecords; ++i )
    {
        fDWG.read ( (char *) &this->fileHeader.paSLRecords[i].byRecordNumber, 1 );
        fDWG.read ( (char *) &this->fileHeader.paSLRecords[i].dSeeker, 4 );
        fDWG.read ( (char *) &this->fileHeader.paSLRecords[i].dSize, 4 );

#ifdef _LIBDWGX_DEBUG
        printf ( "SL Record #%d : %d %d\n", this->fileHeader.paSLRecords[i].byRecordNumber,
                                            this->fileHeader.paSLRecords[i].dSeeker,
                                            this->fileHeader.paSLRecords[i].dSize );
#endif
    }

    delete [] pabyBuf;
}
