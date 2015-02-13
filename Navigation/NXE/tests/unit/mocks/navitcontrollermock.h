#ifndef NAVITCONTROLLERMOCK_H
#define NAVITCONTROLLERMOCK_H

#include "navitcontroller.h"

class NavitControllerMock : public NXE::NavitController {
    GMOCK_METHOD0_(, noexcept,, start, void());
    GMOCK_METHOD0_(, noexcept,, stop, void());
    GMOCK_METHOD2_(, noexcept,, moveBy, void(double, double));
};

#endif // NAVITCONTROLLERMOCK_H

