#include <QApplication>
#include "rjwidget.h"
#ifndef LIB
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RJWidget w;
    w.show();
    
    return a.exec();
}
#else
extern "C"{
    Q_DECL_EXPORT QWidget *Instance(QWidget *p){
        return new RJWidget(p);
    }
    Q_DECL_EXPORT int level(){
        return 0;
    }
}
#endif
