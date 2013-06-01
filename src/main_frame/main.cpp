#include "mainframe.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setEffectEnabled(Qt::UI_AnimateMenu );
    QTranslator translator;
    translator.load("lang_zh.qm", ".");
    app.installTranslator(&translator);
    app.setStyleSheet(
        "QMenu {"
        "background-color: #ABABAB;"
        "border: 1px solid black;"
        "}"
        "QMenu::item {"
        "background-color: transparent;"
        "}"
        "QMenu::item:selected {"
        "background-color: #654321;"
        "}");
    MainFrame w;
    w.showNormal();

    return app.exec();
}
