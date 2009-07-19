cmake_minimum_required(VERSION 2.6)
project(plasma-bibleverse)
 
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )
 
 
# We add our source code here
set(bibleverse_SRCS plasma-bibleverse.cpp versedownloader.cpp web.cpp)
kde4_add_ui_files(bibleverse_SRCS bibleVerseConfig.ui)
qt4_automoc(${bibleverse_SRCS}) 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_bibleverse ${bibleverse_SRCS})
target_link_libraries(plasma_applet_bibleverse
                      ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} QtNetwork)
 
install(TARGETS plasma_applet_bibleverse
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-bibleverse.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
