#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    // Set application metadata
    QApplication::setApplicationName("Aurora");
    QApplication::setApplicationVersion("0.1.0");
    QApplication::setOrganizationName("AbstractMelon");
    QApplication::setOrganizationDomain("abstractmelon.com");
    
    // Set modern style
    QApplication::setStyle("Fusion");
    
    MainWindow window;
    window.show();
    
    return app.exec();
}
