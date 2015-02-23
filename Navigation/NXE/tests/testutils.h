#ifndef TESTUTILS_H
#define TESTUTILS_H

#include <string>
#include "jsonmessage.h"

namespace TestUtils
{
std::string moveByMessage(int x, int y);
std::string zoomByMessage(int factor);
void createNXEConfFile();
}

#endif // TESTUTILS_H
