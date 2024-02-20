#include "pch.h"

#include "measurementwindow.h"

int main(int argc, char *argv[]) {
    QApplication application(argc, argv);

    MeasurementWindow measurementWindow;
    measurementWindow.show();

    return application.exec();
}
