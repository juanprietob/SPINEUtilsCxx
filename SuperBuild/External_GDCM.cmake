if( NOT EXTERNAL_SOURCE_DIRECTORY )
  set( EXTERNAL_SOURCE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/ExternalSources )
endif()

# Make sure this file is included only once by creating globally unique varibles
# based on the name of this included file.
get_filename_component(CMAKE_CURRENT_LIST_FILENAME ${CMAKE_CURRENT_LIST_FILE} NAME_WE)
if(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED)
  return()
endif()
set(${CMAKE_CURRENT_LIST_FILENAME}_FILE_INCLUDED 1)

## External_${extProjName}.cmake files can be recurisvely included,
## and cmake variables are global, so when including sub projects it
## is important make the extProjName and proj variables
## appear to stay constant in one of these files.
## Store global variables before overwriting (then restore at end of this file.)
ProjectDependancyPush(CACHED_extProjName ${extProjName})
ProjectDependancyPush(CACHED_proj ${proj})

# Make sure that the ExtProjName/IntProjName variables are unique globally
# even if other External_${ExtProjName}.cmake files are sourced by
# SlicerMacroCheckExternalProjectDependency
set(extProjName GDCM) #The find_package known name
set(proj        GDCM) #This local name


#Setting VTK_VERSION_MAJOR to its default value if it has not been set before
#set(VTK_VERSION_MAJOR 6 CACHE STRING "Choose the expected VTK major version to build Slicer (5 or 6).")
# Set the possible values of VTK major version for cmake-gui
set_property(CACHE VTK_VERSION_MAJOR PROPERTY STRINGS "5" "6")
if(NOT "${VTK_VERSION_MAJOR}" STREQUAL "5" AND NOT "${VTK_VERSION_MAJOR}" STREQUAL "6")
  set(VTK_VERSION_MAJOR 5 CACHE STRING "Choose the expected VTK major version to build Slicer (5 or 6)." FORCE)
  message(WARNING "Setting VTK_VERSION_MAJOR to '5' as an valid value was specified.")
endif()

set(USE_VTKv5 ON)
set(USE_VTKv6 OFF)
if(${VTK_VERSION_MAJOR} STREQUAL "6")
  set(USE_VTKv5 OFF)
  set(USE_VTKv6 ON)
endif()

if(USE_VTKv6)
  set(${extProjName}_REQUIRED_VERSION "6.1")  #If a required version is necessary, then set this, else leave blank
else()
  set(${extProjName}_REQUIRED_VERSION "5.10")  #If a required version is necessary, then set this, else leave blank
endif()

# Sanity checks
#if(DEFINED ${extProjName}_DIR AND NOT EXISTS ${${extProjName}_DIR})
#  message(FATAL_ERROR "${extProjName}_DIR variable is defined but corresponds to non-existing directory (${${extProjName}_DIR})")
#endif()

# Set dependency list
set(${proj}_DEPENDENCIES "")
set(${PROJECT_NAME}_USE_PYTHONQT OFF)
if (${PROJECT_NAME}_USE_PYTHONQT)
  list(APPEND ${proj}_DEPENDENCIES python)
endif()

# Include dependent projects if any
SlicerMacroCheckExternalProjectDependency(${proj})

if(NOT ( DEFINED "USE_SYSTEM_${extProjName}" AND "${USE_SYSTEM_${extProjName}}" ) )
  #message(STATUS "${__indent}Adding project ${proj}")

  # Set CMake OSX variable to pass down the external project
  set(CMAKE_OSX_EXTERNAL_PROJECT_ARGS)
  if(APPLE)
    list(APPEND CMAKE_OSX_EXTERNAL_PROJECT_ARGS
      -DCMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES}
      -DCMAKE_OSX_SYSROOT=${CMAKE_OSX_SYSROOT}
      -DCMAKE_OSX_DEPLOYMENT_TARGET=${CMAKE_OSX_DEPLOYMENT_TARGET}
      -DVTK_REQUIRED_OBJCXX_FLAGS:STRING="")
  endif()

  set(${proj}_CMAKE_OPTIONS
      -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/${proj}-install
      -DGDCM_BUILD_TESTING:BOOL=OFF
      -DGDCM_BUILD_APPLICATIONS:BOOL=ON
      -DGDCM_USE_SYSTEM_OPENSSL:BOOL=ON
      ${EXTERNAL_PROJECT_OPTIONAL_ARGS}
    )

  set(${proj}_GIT_TAG "58b53bae5e729b62d374d1c732d8ae9832842f06")
  set(${proj}_REPOSITORY ${git_protocol}://github.com/juanprietob/GDCM.git)


 SET(GDCM_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/${proj}-install/include" CACHE TYPE PATH)
 SET(GDCM_LINK_DIRECTORIES "${CMAKE_CURRENT_BINARY_DIR}/${proj}-install/lib" CACHE TYPE PATH)


  ExternalProject_Add(${proj}
    GIT_REPOSITORY ${${proj}_REPOSITORY}
    GIT_TAG ${${proj}_GIT_TAG}
    SOURCE_DIR ${EXTERNAL_SOURCE_DIRECTORY}/${proj}
    BINARY_DIR ${proj}-build
    LOG_CONFIGURE 0  # Wrap configure in script to ignore log output from dashboards
    LOG_BUILD     0  # Wrap build in script to to ignore log output from dashboards
    LOG_TEST      0  # Wrap test in script to to ignore log output from dashboards
    LOG_INSTALL   0  # Wrap install in script to to ignore log output from dashboards
    ${cmakeversion_external_update} "${cmakeversion_external_update_value}"
    CMAKE_GENERATOR ${gen}
    CMAKE_ARGS
      ${CMAKE_OSX_EXTERNAL_PROJECT_ARGS}
      ${COMMON_EXTERNAL_PROJECT_ARGS}
      ${${proj}_CMAKE_OPTIONS}
## We really do want to install in order to limit # of include paths INSTALL_COMMAND ""
    DEPENDS
      ${${proj}_DEPENDENCIES}
    )

else()
  if(${USE_SYSTEM_${extProjName}})

    find_path(GDCM_INCLUDE_DIRECTORIES gdcm-2.5/gdcmReader.h 
      HINTS 
      /usr/local/include/
    )
    #SET(GDCM_INCLUDE_DIRECTORIES CACHE TYPE PATH)
    #find_package(${extProjName})
    message("USING the system ${extProjName}, set ${extProjName}_DIR=${${extProjName}_DIR}")
  endif()
  # The project is provided using ${extProjName}_DIR, nevertheless since other
  # project may depend on ${extProjName}, let's add an 'empty' one
  SlicerMacroEmptyExternalProject(${proj} "${${proj}_DEPENDENCIES}")
endif()

list(APPEND ${CMAKE_PROJECT_NAME}_SUPERBUILD_EP_VARS ${extProjName}_DIR:PATH)

ProjectDependancyPop(CACHED_extProjName extProjName)
ProjectDependancyPop(CACHED_proj proj)
