


SET(EXE_NAME testReadSPINEContour)
SET(${EXE_NAME}_LIBRARIES ${VTK_LIBRARIES} ${XERCES_LIBRARIES} SPINEToolsIO)

add_executable(${EXE_NAME} ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})




SET(EXE_NAME testContourInterpolation)
SET(${EXE_NAME}_LIBRARIES  SPINEToolsIO SPINEToolsInterpolation)

add_executable(${EXE_NAME} ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})


SET(EXE_NAME testContourRegistration)
SET(${EXE_NAME}_LIBRARIES  SPINEToolsIO SPINEToolsInterpolation)

add_executable(${EXE_NAME} ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})

SET(EXE_NAME testWfbplot)
SET(${EXE_NAME}_LIBRARIES  Wfbplot)

add_executable(${EXE_NAME} ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})

SET(EXE_NAME testSPINEWfbplot)
SET(${EXE_NAME}_LIBRARIES  Wfbplot ${VTK_LIBRARIES})

add_executable(${EXE_NAME} ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
