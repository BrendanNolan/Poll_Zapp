QT -= \
    core \
    gui

TARGET = utils_geometric
TEMPLATE = lib
CONFIG += static

HEADERS += \
    geom_defs.h \
    Point.h \
    Polygon.h \
    IPolygonLayoutEngine.h \
    PolygonInflatingPositioningEngine.h

SOURCES += \
    Point.cpp \
    Polygon.cpp \
    PolygonInflatingPositioningEngine.cpp
