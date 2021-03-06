#ifndef POLYGON_H
#define POLYGON_H

#include <vector>

#include "Point.h"

namespace geom
{

class Polygon
{
public:
    Polygon() = default;
    Polygon(const std::vector<Point>& points);

    void inflate(double inflationFactor);
    void moveTo(const Point& toPoint);
    void translate(const Point& translationPoint);

    const std::vector<Point>& points() const;
    std::vector<Point>& points();
    void setPoints(const std::vector<Point>& points);

    bool isValid() const;

    int edgeCount() const;

private:
    std::vector<Point> points_;
};

Point centroid(const Polygon& polygon);
double area(const Polygon& polygon);
double width(const Polygon& polygon);
double height(const Polygon& polygon);
std::vector<double> xCoords(const Polygon& polygon);
std::vector<double> yCoords(const Polygon& polygon);

}// namespace geom

#endif// POLYGON_H
