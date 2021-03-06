SET (REQUIRED_SWORD_VERSION 1.6.0)

# This module looks for installed sword
#
# It will define the following values
# SWORD_INCLUDE_DIR
# SWORD_LIBRARY
# SWORD_LIBRARY_DIR

IF(MSVC)
	SET(SWORD_WIN32_LIBRARY_PATH ../sword/lib/vcppmake/vc8/ICUDebug)
	SET(SWORD_WIN32_INCLUDE_PATH ../sword/include)
ENDIF(MSVC)

SET(TRIAL_LIBRARY_PATHS
	$ENV{SWORD_HOME}/lib${LIB_SUFFIX}
	${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX}
	/usr/local/lib${LIB_SUFFIX}
	/opt/local/lib${LIB_SUFFIX}
	/usr/lib${LIB_SUFFIX}
	/usr/lib64
	/usr/pkg/lib${LIB_SUFFIX}
	${SWORD_WIN32_LIBRARY_PATH}
)
SET(TRIAL_INCLUDE_PATHS
	$ENV{SWORD_HOME}/include/sword
	$ENV{SWORD_HOME}/include
	${CMAKE_INSTALL_PREFIX}/include/sword
	${CMAKE_INSTALL_PREFIX}/include
	/usr/local/include/sword
	/usr/local/include
	/usr/include/sword
	/usr/include
	/sw/include/sword
	/sw/include
	/usr/pkg/include/sword
	/usr/pkg/include
	${SWORD_WIN32_INCLUDE_PATH}
)

IF(MSVC)
	FIND_LIBRARY(SWORD_LIBRARY NAMES libsword PATHS ${TRIAL_LIBRARY_PATHS})
ELSE(MSVC)
	FIND_LIBRARY(SWORD_LIBRARY sword NAMES libsword PATHS ${TRIAL_LIBRARY_PATHS})
ENDIF(MSVC)

IF (SWORD_LIBRARY)
	MESSAGE(STATUS "Found Sword library: ${SWORD_LIBRARY}")
#	SET(SWORD_LIBRARY "sword")
ELSE (SWORD_LIBRARY)
	MESSAGE(STATUS "Could not find the Sword library.")
	SET(SWORD_NOT_FOUND TRUE)
ENDIF (SWORD_LIBRARY)

FIND_PATH(SWORD_LIBRARY_DIR	NAMES libsword.a libsword.la libsword.so libsword.lib PATHS ${TRIAL_LIBRARY_PATHS} ENV CMAKE_LIBRARY_PATH)
IF (SWORD_LIBRARY_DIR)
	MESSAGE(STATUS "Found Sword library dir: ${SWORD_LIBRARY_DIR}")
ELSE (SWORD_LIBRARY_DIR)
	MESSAGE(STATUS "Could not find the Sword library dir.")
	SET(SWORD_NOT_FOUND TRUE)
ENDIF (SWORD_LIBRARY_DIR)

FIND_PATH(SWORD_INCLUDE_DIR	NAMES swmgr.h PATHS ${TRIAL_INCLUDE_PATHS})
IF (SWORD_INCLUDE_DIR)
	MESSAGE(STATUS "Found Sword include dir: ${SWORD_INCLUDE_DIR}")
ELSE (SWORD_INCLUDE_DIR)
	MESSAGE(STATUS "Sword include dir could not be found.")
	SET(SWORD_NOT_FOUND TRUE)
	SET(SWORD_INCLUDE_DIR TRUE)
ENDIF (SWORD_INCLUDE_DIR)

#
# Check for minimum Sword version
#
IF (SWORD_NOT_FOUND)
	
ELSE (SWORD_NOT_FOUND)
	MACRO(CHECK_SWORD_VERSION VERSION)
		IF(MSVC)
			SET(CHECK_SWORD_VERSION_DEFINES "/DSWUSINGDLL")
		ENDIF(MSVC)
		TRY_RUN(SWVERSIONTEST_RUN_RESULT SWVERSIONTEST_COMPILE_RESULT
			${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sword_version_compare.cpp
			CMAKE_FLAGS
				"-DINCLUDE_DIRECTORIES:STRING=${SWORD_INCLUDE_DIR}"
				"-DLINK_DIRECTORIES:STRING=${SWORD_LIBRARY_DIR}"
				"-DLINK_LIBRARIES:STRING=${SWORD_LIBRARY}"
				${SWORD_LIBRARY}
			COMPILE_DEFINITIONS
				${CHECK_SWORD_VERSION_DEFINES}
			COMPILE_OUTPUT_VARIABLE SWVERSIONTEST_COMPILE_OUTPUT
			RUN_OUTPUT_VARIABLE SWVERSIONTEST_RUN_OUTPUT
			ARGS ${VERSION}
		)
		IF(NOT SWVERSIONTEST_COMPILE_RESULT)
			MESSAGE(FATAL_ERROR "Sword version check program could NOT be compiled: ${SWVERSIONTEST_COMPILE_OUTPUT}")
		ENDIF(NOT SWVERSIONTEST_COMPILE_RESULT)
	ENDMACRO(CHECK_SWORD_VERSION VERSION)

	MESSAGE(STATUS "Checking for required Sword version ${REQUIRED_SWORD_VERSION}...")
	CHECK_SWORD_VERSION(${REQUIRED_SWORD_VERSION})

	IF(SWVERSIONTEST_RUN_RESULT EQUAL 0)
		MESSAGE(STATUS "Installed Sword version is ok. Check program said: ${SWVERSIONTEST_RUN_OUTPUT}")
	ELSE(SWVERSIONTEST_RUN_RESULT EQUAL 0)
		MESSAGE(FATAL_ERROR "Installed Sword version is NOT ok! Check program said: ${SWVERSIONTEST_RUN_OUTPUT}")
	ENDIF(SWVERSIONTEST_RUN_RESULT EQUAL 0)

	#
	# Sword linker flag detection
	#
	MACRO(CHECK_SWORD_LINK_LIBRARIES FLAGS)
		IF(MSVC)
			SET(CHECK_SWORD_CXX_FLAGS "/Zc:wchar_t- /MDd")
			SET(CHECK_SWORD_LINK_DEFINES "/DSWUSINGDLL")
		ELSE(MSVC)
			SET(CHECK_SWORD_CXX_FLAGS "")
			SET(CHECK_SWORD_LINK_DEFINES "")
		ENDIF(MSVC)
		TRY_COMPILE(SWLINKER_CHECK_COMPILE_RESULT
		${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/sword_linker_check.cpp
		CMAKE_FLAGS
			"-DINCLUDE_DIRECTORIES:STRING=${SWORD_INCLUDE_DIR}"
			"-DLINK_DIRECTORIES:STRING=${SWORD_LIBRARY_DIR};${CLUCENE_LIBRARY_DIR}"
			"-DLINK_LIBRARIES:STRING=${FLAGS}"
			"-DCMAKE_CXX_FLAGS_DEBUG:STRING=${CHECK_SWORD_CXX_FLAGS}"
		COMPILE_DEFINITIONS
			${CHECK_SWORD_LINK_DEFINES}
		OUTPUT_VARIABLE SWLINKER_CHECK_COMPILE_OUTPUT
	)
	ENDMACRO(CHECK_SWORD_LINK_LIBRARIES FLAGS)

	#CURL_LIBRARIES and ICU_LIBRARIES are optional, empty if not found
	CHECK_SWORD_LINK_LIBRARIES("${CLUCENE_LIBRARY};${SWORD_LIBRARY};${CURL_LIBRARIES};${ICU_LIBRARIES};${ICU_I18N_LIBRARIES};${ZLIB_LIBRARIES}")

	IF(SWLINKER_CHECK_COMPILE_RESULT)
		MESSAGE(STATUS "Sword linker check compiled ok.")
		# SWORD_LIBRARY can stay unchanged
	ELSE(SWLINKER_CHECK_COMPILE_RESULT)
		MESSAGE(STATUS "Sword linker check could NOT be compiled. It seems that you need additional libraries for the linker.")
		MESSAGE(STATUS "Here is the detailed output of the compilation and linking process:")
		MESSAGE(FATAL_ERROR "${SWLINKER_CHECK_COMPILE_OUTPUT}")
	ENDIF(SWLINKER_CHECK_COMPILE_RESULT)

	add_definitions(-DUSE_SWORD="\"use\"")
	MARK_AS_ADVANCED(
		SWORD_INCLUDE_DIR
		SWORD_LIBRARY
		SWORD_LIBRARY_DIR
	)
ENDIF (SWORD_NOT_FOUND)