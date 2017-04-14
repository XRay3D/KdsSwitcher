#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>
//#include "StyleLoader/styleloader.h"

int main(int argc, char* argv[])
{

    QApplication a(argc, argv);

    //    qDebug() << QStyleFactory::keys();
    //    // Создаём палитру для тёмной темы оформления
    //    QPalette darkPalette;

    //    darkPalette.setColor(QPalette::Button, Qt::white);//black);
    //    darkPalette.setColor(QPalette::Light, Qt::white);//white);
    //    darkPalette.setColor(QPalette::Midlight, Qt::white);//darkGray);
    //    darkPalette.setColor(QPalette::Dark, Qt::white);//gray);
    //    darkPalette.setColor(QPalette::Mid, Qt::white);//lightGray);
    //    darkPalette.setColor(QPalette::Text, Qt::white);//red);
    //    darkPalette.setColor(QPalette::BrightText, Qt::white);//green);
    //    darkPalette.setColor(QPalette::ButtonText, Qt::white);//blue);
    //    darkPalette.setColor(QPalette::Base, Qt::white);//cyan);
    //    darkPalette.setColor(QPalette::Window, Qt::white);//magenta);
    //    darkPalette.setColor(QPalette::Shadow, Qt::white);//yellow);
    //    darkPalette.setColor(QPalette::Highlight, Qt::white);//darkRed);
    //    darkPalette.setColor(QPalette::HighlightedText, Qt::white);//darkGreen);
    //    darkPalette.setColor(QPalette::Link, Qt::white);//darkBlue);
    //    darkPalette.setColor(QPalette::LinkVisited, Qt::white);//darkCyan);
    //    darkPalette.setColor(QPalette::AlternateBase, Qt::white);//darkMagenta);
    //    darkPalette.setColor(QPalette::NoRole, Qt::white);//darkYellow);
    //    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);//black);
    //    darkPalette.setColor(QPalette::ToolTipText, Qt::white);//white);
    //    darkPalette.setColor(QPalette::Foreground, Qt::white);//white); //= WindowText,
    //    darkPalette.setColor(QPalette::Background, Qt::white);//darkGray); //= Window

    //    // Настраиваем палитру для цветовых ролей элементов интерфейса
    //    //    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    //    //    darkPalette.setColor(QPalette::WindowText, Qt::white);//white);
    //    //    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    //    //    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    //    //    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);//white);
    //    //    darkPalette.setColor(QPalette::ToolTipText, Qt::white);//white);
    //    //    darkPalette.setColor(QPalette::Text, Qt::white);//white);
    //    //    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    //    //    darkPalette.setColor(QPalette::ButtonText, Qt::white);//white);
    //    //    darkPalette.setColor(QPalette::BrightText, Qt::white);//red);
    //    //    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    //    //    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    //    //    darkPalette.setColor(QPalette::HighlightedText, Qt::white);//black);

    //    // Устанавливаем данную палитру
    //    QApplication::setPalette(darkPalette);
    //    QApplication::setStyle("Fusion");
    MainWindow w;

    //StyleLoader::attach("c:/myStyle.qss", QKeySequence("/"));

    w.show();

    return a.exec();
}
