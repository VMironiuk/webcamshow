#include "streamwidget.h"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    StreamWidget widget;
    widget.show();

    return app.exec();
}
