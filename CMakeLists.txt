cmake_minimum_required(VERSION 2.8)

project(SPINEUtilsCXX)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

#find_package(GDCM REQUIRED)
#include(${GDCM_USE_FILE})


#link_directories(/home/prieto/NetBeansProjects/install/lib)

add_executable(ImageSeriesReadWrite MACOSX_BUNDLE appli/ImageSeriesReadWrite.cxx)
target_link_libraries(ImageSeriesReadWrite ${ITK_LIBRARIES})

add_executable(ImageSeriesReadWriteDICOM MACOSX_BUNDLE appli/ImageSeriesReadWriteDICOM.cxx)
target_link_libraries(ImageSeriesReadWriteDICOM ${ITK_LIBRARIES})


#include_directories(/usr/include/xercesc/)
#link_directories(/usr/lib/)
#set(XERCES_LIBRARIES xerces-c-3.1)
#add_executable(SimpleRegionGrowingSegmentation MACOSX_BUNDLE appli/SimpleRegionGrowingSegmentation.cxx)
#target_link_libraries(SimpleRegionGrowingSegmentation ${ITK_LIBRARIES} ${XERCES_LIBRARIES})


