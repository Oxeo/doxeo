#include <QCoreApplication>
#include "doxeomonitor.h"

int main(int argc, char *argv[])
{
    DoxeoMonitor doxeoMonitor(argc, argv);

    return doxeoMonitor.start();
}
