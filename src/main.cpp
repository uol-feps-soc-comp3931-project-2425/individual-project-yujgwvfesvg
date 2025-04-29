#include <QApplication>
#include "frontend.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    HyperbolicityApp window;
    window.show();
    return app.exec();
}
