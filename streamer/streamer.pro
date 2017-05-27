TEMPLATE = app
TARGET = ../bin/streamer
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
INCLUDEPATH += /usr/include/gstreamer-1.0 \
               /usr/include/glib-2.0 \
               /usr/lib/i386-linux-gnu/glib-2.0/include \
               mediagraph/graph \
               mediagraph/output

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += gstreamer-1.0 \
                   gstreamer-app-1.0 \
                   gstreamer-plugins-base-1.0 \
                   gstreamer-video-1.0

SOURCES += main.cpp \
    streamer.cpp \
    mediagraph/graph/abstractgraph.cpp \
    mediagraph/graph/previewgraph.cpp \
    mediagraph/graph/rtmpgraph.cpp \
    mediagraph/graph/rtmppreviewgraph.cpp \
    mediagraph/graph/videotestgraph.cpp \
    mediagraph/output/abstractoutputbin.cpp \
    mediagraph/output/videopreviewbin.cpp \
    mediagraph/output/rtmpstreamingbin.cpp

HEADERS += \
    streamer.h \
    mediagraph/graph/abstractgraph.h \
    mediagraph/graph/previewgraph.h \
    mediagraph/graph/rtmpgraph.h \
    mediagraph/graph/rtmppreviewgraph.h \
    mediagraph/graph/videotestgraph.h \
    mediagraph/output/abstractoutputbin.h \
    mediagraph/output/videopreviewbin.h \
    mediagraph/output/rtmpstreamingbin.h
