# libOpenCAD
OpenSource library under X11/MIT license for everyday use. Under development, but some of basic functionality is already available.

All help will be very good for project. A lot of TODO's in code, so you can fix something you want.

API
---
libdwgx typical usage pipeline is shown below:
Open CAD file:
```cpp
#include <iostream>
#include "libopencad_api.h"
using namespace libopencad;

CADFile * opendwg = OpenCADFile("/path/to/example.dwg/dxf");
```

From now on, you can access geometries like this:
```cpp
int geometries_count = opendwg->getGeometriesCount();
for(auto i = 0; i < geometries_count; ++i)
{
    CADGeometries::CADGeometry * geom = dwgfile->getGeometry (i);
    // now, geom->sGeometryType stores a type of returned geometry. Then,
    // you have to cast it to this class, eg.
    if ( geom->sGeometryType == CADGeometries::CADGeometryType::CIRCLE )
    {
        CADGeometries::Circle * circle = geom;
        // There you go. Now you can get all the data you want, eg.
        std::cout << circle->dfCenterX;
        std::cout << circle->dfCenterY;
        std::cout << circle->dfCenterZ;
        std::cout << circle->dfRadius;
    }
}
```

Now supported CAD files version are:

1. R2000 (read-only). Its under development still, and a lot of geometries are missed, but it will all come with time.
2. Next up on the list: R2004.

Build status
------------
Linux: [![Build Status](https://travis-ci.org/sandyre/libopencad.svg?branch=master)](https://travis-ci.org/sandyre/libopencad)
