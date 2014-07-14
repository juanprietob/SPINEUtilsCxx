cmake_minimum_required(VERSION 2.8)

project(SPINEUtilsCXX)


FIND_PACKAGE(VTK)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#find_package(GDCM REQUIRED)
#include(${GDCM_USE_FILE})


include_directories(/usr/local/include/)
set(XERCES_LIBRARIES xerces-c-3.1)


INCLUDE(cmake/UserMacros.cmake)

CREA_CPDIR(${PROJECT_SOURCE_DIR}/data ${PROJECT_BINARY_DIR}/data)

message(${PROJECT_SOURCE_DIR}/lib/SPINEToolsIO)

INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/lib/SPINEToolsIO
        ${PROJECT_SOURCE_DIR}/lib/SPINEToolsInterpolation
        ${PROJECT_SOURCE_DIR}/lib/Wfbplot
)

LINK_DIRECTORIES(
        ${PROJECT_BINARY_DIR}/lib/
)

SUBDIRS(lib)
SUBDIRS(appli)

OPTION(BUILD_ANONYMIZE "Build test ?" TRUE)
OPTION(BUILD_TEST "Build test ?" OFF)
IF(BUILD_TEST)
	SUBDIRS(test)
ENDIF(BUILD_TEST)

