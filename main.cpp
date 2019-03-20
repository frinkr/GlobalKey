#include <iostream>
#include "GKApplication.h"
#include "GKAboutDialog.h"
int main(int argc, char *argv[]) {
    GKApplication app(argc, argv);
    //GKAboutDialog dialog;
    //dialog.exec();
    return app.exec();
}
