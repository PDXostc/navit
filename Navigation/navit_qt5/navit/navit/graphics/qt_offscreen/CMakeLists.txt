add_definitions(-std=c++11)
module_add_library(graphics_qt_offscreen graphics_qt_offscreen.cpp)
qt5_use_modules(graphics_qt_offscreen Widgets Gui OpenGL)
