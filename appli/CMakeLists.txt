

add_executable(ImageSeriesReadWrite MACOSX_BUNDLE ImageSeriesReadWrite.cxx)
target_link_libraries(ImageSeriesReadWrite ${ITK_LIBRARIES})

add_executable(ImageSeriesReadWriteDICOM MACOSX_BUNDLE ImageSeriesReadWriteDICOM.cxx)
target_link_libraries(ImageSeriesReadWriteDICOM ${ITK_LIBRARIES})

add_executable(SimpleRegionGrowingSegmentation MACOSX_BUNDLE SimpleRegionGrowingSegmentation.cxx)
target_link_libraries(SimpleRegionGrowingSegmentation ${ITK_LIBRARIES} ${XERCES_LIBRARIES})


SET(EXE_NAME SPINEContourRegistration)
SET(${EXE_NAME}_LIBRARIES  SPINEToolsIO SPINEToolsInterpolation)

add_executable(${EXE_NAME} MACOSX_BUNDLE ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})


