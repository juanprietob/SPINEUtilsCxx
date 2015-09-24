
include(cmake/FindJSON.cmake)

message(${JSON_INCLUDE_DIR})

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

FIND_PACKAGE(VTK)
IF(${VTK_FOUND})
	include(${VTK_USE_FILE})
ENDIF(${VTK_FOUND})


INCLUDE_DIRECTORIES(${GDCM_INCLUDE_DIRECTORIES})


#include_directories(/usr/local/include/)
set(XERCES_LIBRARIES xerces-c-3.1)


INCLUDE(cmake/UserMacros.cmake)

CREA_CPDIR(${PROJECT_SOURCE_DIR}/src/data ${PROJECT_BINARY_DIR}/data)


INCLUDE_DIRECTORIES(
	${PROJECT_SOURCE_DIR}/src/lib/SPINEToolsIO
    ${PROJECT_SOURCE_DIR}/src/lib/SPINEToolsInterpolation
    ${PROJECT_SOURCE_DIR}/src/lib/Wfbplot
    ${PROJECT_SOURCE_DIR}/src/lib/SPINEFiltersITK
    ${PROJECT_SOURCE_DIR}/src/lib/SPINEFiltersVTK
)

LINK_DIRECTORIES(
    ${PROJECT_BINARY_DIR}/lib/
)

add_subdirectory(lib)
add_subdirectory(appli)
add_subdirectory(install)

OPTION(BUILD_ANONYMIZE "Build Anonymize ?" TRUE)
OPTION(BUILD_TEST "Build test ?" OFF)
IF(BUILD_TEST)
	add_subdirectory(test)
ENDIF(BUILD_TEST)

