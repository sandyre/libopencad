#ifndef LIBDWGX_DWG_GEOMETRIES_H
#define LIBDWGX_DWG_GEOMETRIES_H

#include <string>

namespace libdwgx
{
    namespace DWGGeometries
    {

        class Geometry
        {
        public:
            std::string sGeometryType;
            double      dfThickness;
        };

        class Point : public Geometry
        {
        public:
            Point () : dfPointX (0.0f),
                       dfPointY (0.0f),
                       dfPointZ (0.0f),
                       dfExtrusionX (0.0f),
                       dfExtrusionY (0.0f),
                       dfExtrusionZ (0.0f),
                       dfXAxisAng (0.0f)
            {
                sGeometryType = "Point";
            }

            double dfPointX;
            double dfPointY;
            double dfPointZ;
            double dfExtrusionX;
            double dfExtrusionY;
            double dfExtrusionZ;
            double dfXAxisAng;
        };

        class Polyline : public Geometry
        {
        public:
            Polyline ()
            {

            }
        };

        class Circle : public Geometry
        {
        public:
            Circle () : dfCenterX (0.0f),
                        dfCenterY (0.0f),
                        dfCenterZ (0.0f),
                        dfExtrusionX (0.0f),
                        dfExtrusionY (0.0f),
                        dfExtrusionZ (0.0f),
                        dfRadius (0.0f)
            {
                sGeometryType = "Circle";
            }

            double dfCenterX;
            double dfCenterY;
            double dfCenterZ;
            double dfExtrusionX;
            double dfExtrusionY;
            double dfExtrusionZ;
            double dfRadius;
        };

    }
}

#endif //LIBDWGX_DWG_GEOMETRIES_H
