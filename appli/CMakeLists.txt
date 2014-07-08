

add_executable(ImageSeriesReadWrite MACOSX_BUNDLE ImageSeriesReadWrite.cxx)
target_link_libraries(ImageSeriesReadWrite ${ITK_LIBRARIES})

add_executable(ImageSeriesReadWriteDICOM MACOSX_BUNDLE ImageSeriesReadWriteDICOM.cxx)
target_link_libraries(ImageSeriesReadWriteDICOM ${ITK_LIBRARIES})

add_executable(SimpleRegionGrowingSegmentation MACOSX_BUNDLE SimpleRegionGrowingSegmentation.cxx)
target_link_libraries(SimpleRegionGrowingSegmentation ${ITK_LIBRARIES} ${XERCES_LIBRARIES})


