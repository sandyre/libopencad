#ifndef __DWG_R2000_H__
#define __DWG_R2000_H__

#include "libdwgx.h"
#include "dwg_base.h"

#include <map>
#include <string>
#include <vector>

using std::map;
using std::string;
using std::vector;
using std::pair;

struct DWG2000_CLASS
{
    short CLASSNUM; // BITSHORT
    short VERSION; // BITSHORT
    string APPNAME; // TV
    string CPLUSPLUSCLASSNAME; // TV
    string CLASSDXFNAME; // TV
    bool WASAZOMBIE; // BIT
    short ITEMCLASSID; // BITSHORT
};

struct DWG2000_HEADER_VARIABLES
{
    double Unknown1;
    double Unknown2;
    double Unknown3;
    double Unknown4;
    string Unknown5;
    string Unknown6;
    string Unknown7;
    string Unknown8;
    long   Unknown9;
    long   Unknown10;
    bool DIMASO;
    bool DIMSHO;
    bool PLINEGEN;
    bool ORTHOMODE;
    bool REGENMODE;
    bool FILLMODE;
    bool QTEXTMODE;
    bool PSLTSCALE;
    bool LIMCHECK;
    bool MIRRTEXT;
    bool WORLDVIEW;
    bool TILEMODE;
    bool PLIMCHECK;
    bool VISRETAIN;
    bool DISPSILH;
    bool PELLIPSE;
    short PROXYGRAPHICS;
    short TREEDEPTH;
    short LUNITS;
    short LUPREC;
    short AUNITS;
    short AUPREC;
    short ATTMODE;
    short PDMODE;
    short USERI1;
    short USERI2;
    short USERI3;
    short USERI4;
    short USERI5;
    short SPLINESEGS;
    short SURFU;
    short SURFV;
    short SURFTYPE;
    short SURFTAB1;
    short SURFTAB2;
    short SPLINETYPE;
    short SHADEDGE;
    short SHADEDIF;
    short UNITMODE;
    short MAXACTVP;
    short ISOLINES;
    short CMLJUST;
    short TEXTQLTY;
    double LTSCALE;
    double TEXTSIZE;
    double TRACEWID;
    double SKETCHING;
    double FILLETRAD;
    double THICKNESS;
    double ANGBASE;
    double PDSIZE;
    double PLINEWID;
    double USERR1;
    double USERR2;
    double USERR3;
    double USERR4;
    double USERR5;
    double CHAMFERA;
    double CHAMFERB;
    double CHAMFERC;
    double CHAMFERD;
    double FACETRES;
    double CMLSCALE;
    double CELTSCALE;
    string MENUNAME;
    long   TDCREATE1; // TODO: WTF?
    long   TDCREATE2;
    long   TDUPDATE1;
    long   TDUPDATE2;

    long   TDINDWG1;
    long   TDINDWG2;
    long   TDUSRTIMER1;
    long   TDUSRTIMER2;

    short  CECOLOR;
};

struct DWGR2000_FILE_HEADER
{
    int32_t dImageSeeker;
    int16_t dCodePage;

    vector < SLRecord >  SLRecords;
    map < string, char > header_variables;
};

typedef pair< long long, long long > ObjHandleOffset;

class DWGFileR2000 : public DWGFile
{
public:
    DWGFileR2000() {}
    ~DWGFileR2000() override {}

    int ReadHeader() override;
    int ReadClassesSection() override;
    int ReadObjectMap() override;
    int ReadObject() override;
private:
    DWG2000_CLASS ReadClass( const char * input_array, size_t& bitOffset );

    vector < ObjHandleOffset > object_map;
    vector < DWG2000_CLASS > custom_classes;
    DWGR2000_FILE_HEADER fileHeader;
};

#endif
