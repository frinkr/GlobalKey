#include "GKApplication.h"
#include "GKAboutDialog.h"
#include "GKDialog.h"
int main(int argc, char *argv[]) {
    GKApplication app(argc, argv);
    GKDialog dialog;
    dialog.exec();
    return app.exec();
}
