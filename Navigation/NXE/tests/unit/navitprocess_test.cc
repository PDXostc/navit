
#include "navitprocessimpl.h"
#include "nxe_extension.h"

#include <iostream>
#include <chrono>
#include <thread>

int main(int argc, char *argv[])
{
    NXE::NavitProcessImpl p;
    p.setProgramPath("/home/bata/builds/navit-qt5/navit");
    std::cout << p.isRunning() << std::endl;
    p.start();
    std::cout << p.isRunning() << std::endl;

    std::chrono::milliseconds dura( 60 * 1000 * 1000 );
    std::this_thread::sleep_for( dura );
    return 0;
}
