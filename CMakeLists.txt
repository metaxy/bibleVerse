project(plasma-bibleverse)
cmake_minimum_required(VERSION 2.6)
# Find the required Libaries
find_package(KDE4 REQUIRED)
include(KDE4Defaults)
 
add_definitions (${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(
   ${CMAKE_SOURCE_DIR}
   ${CMAKE_BINARY_DIR}
   ${KDE4_INCLUDES}
   )
#add sword
SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")   

IF (SWORD_NOT_FOUND)
	MESSAGE(STATUS "Compiling without sword support.")
ELSE (SWORD_NOT_FOUND)
FIND_PACKAGE(Sword) 
INCLUDE_DIRECTORIES(
	${CMAKE_CURRENT_BINARY_DIR}
	${SWORD_INCLUDE_DIR}
)
LINK_DIRECTORIES(
	${SWORD_LIBRARY_DIR}
)
ENDIF (SWORD_NOT_FOUND)

set(bibleverse_SRCS src/plasma-bibleverse.cpp src/versedownloader.cpp)
kde4_add_ui_files(bibleverse_SRCS src/bibleVerseGeneralConfig.ui src/bibleVerseViewConfig.ui)
qt4_automoc(${bibleverse_SRCS}) 
# Now make sure all files get to the right place
kde4_add_plugin(plasma_applet_bibleverse ${bibleverse_SRCS})
IF (SWORD_NOT_FOUND)

target_link_libraries(plasma_applet_bibleverse
			${KDE4_PLASMA_LIBS} 
			${KDE4_KDEUI_LIBS}
            ${KDE4_KPARTS_LIBS})
ELSE (SWORD_NOT_FOUND)

target_link_libraries(plasma_applet_bibleverse
			${KDE4_PLASMA_LIBS} 
			${KDE4_KDEUI_LIBS}
            ${KDE4_KPARTS_LIBS}
			${CURL_LIBRARIES} # optional, empty if not found
			${ICU_LIBRARIES} # optional, empty if not found
			${ICU_I18N_LIBRARIES} # optional, empty if not found
			${ZLIB_LIBRARIES} # optional, empty if not found
			${SWORD_LIBRARY})
			
ENDIF (SWORD_NOT_FOUND)

 
install(TARGETS plasma_applet_bibleverse
        DESTINATION ${PLUGIN_INSTALL_DIR})
 
install(FILES plasma-applet-bibleverse.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})
