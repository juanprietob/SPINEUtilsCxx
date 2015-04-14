
FILE(GLOB_RECURSE NAMES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/*.sh)
FOREACH( NAME  ${NAMES})
  CONFIGURE_FILE(
      ${CMAKE_CURRENT_SOURCE_DIR}/${NAME}
      ${PROJECT_BINARY_DIR}/appli/${NAME}
      COPYONLY )
ENDFOREACH(NAME)


add_executable(ImageSeriesReadWrite   ImageSeriesReadWrite.cxx)
target_link_libraries(ImageSeriesReadWrite ${ITK_LIBRARIES})

add_executable(SimpleRegionGrowingSegmentation   SimpleRegionGrowingSegmentation.cxx)
target_link_libraries(SimpleRegionGrowingSegmentation ${ITK_LIBRARIES} ${XERCES_LIBRARIES})

SET(EXE_NAME ImageSeriesReadWriteDICOM)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINENii2Mnc)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINENii2StreamMnc)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINENii2Init)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINEBBImage2Nii)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES} ${JSON_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINEImageLabelVolume)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES} ${JSON_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINEImageLabelOverlapMeasure)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES} ${JSON_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

SET(EXE_NAME SPINERoiFilter)
add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
target_link_libraries(${EXE_NAME} ${ITK_LIBRARIES} ${JSON_LIBRARIES})
install(TARGETS ${EXE_NAME}
        RUNTIME
        DESTINATION bin)

IF(${VTK_FOUND})

    SET(EXE_NAME SPINEAppendContours)
    SET(${EXE_NAME}_LIBRARIES  SPINEToolsIO)
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)

    SET(EXE_NAME SPINEContourRegistration)
    SET(${EXE_NAME}_LIBRARIES  SPINEToolsIO SPINEToolsInterpolation Wfbplot SPINEFiltersVTK ${ITK_LIBRARIES})
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)


    SET(EXE_NAME SPINEWfbplot)
    SET(${EXE_NAME}_LIBRARIES  Wfbplot SPINEToolsIO)
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)

    SET(EXE_NAME SPINEContourArea)
    SET(${EXE_NAME}_LIBRARIES SPINEToolsIO SPINEToolsInterpolation)
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)        

    SET(EXE_NAME SPINEContourInfo)
    SET(${EXE_NAME}_LIBRARIES SPINEToolsIO SPINEToolsInterpolation Wfbplot SPINEFiltersVTK)
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)

    SET(EXE_NAME SPINEBoxplotsArea)
    SET(${EXE_NAME}_LIBRARIES SPINEToolsIO SPINEToolsInterpolation Wfbplot ${ITK_LIBRARIES})
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)

    SET(EXE_NAME SPINEImageData)
    SET(${EXE_NAME}_LIBRARIES ${VTK_LIBRARIES})
    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
    install(TARGETS ${EXE_NAME}
            RUNTIME
            DESTINATION bin)


ENDIF(${VTK_FOUND})

IF(${BUILD_ANONYMIZE})
    SET(EXE_NAME SPINEAnonymize)
    SET(${EXE_NAME}_LIBRARIES  gdcmCommon gdcmDSED gdcmMEXD gdcmcharls gdcmjpeg12 gdcmjpeg8 gdcmuuid socketxx gdcmDICT gdcmIOD gdcmMSFF gdcmexpat gdcmjpeg16 gdcmopenjpeg gdcmzlib)

    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})

    SET(EXE_NAME SPINEAnonymizeReadIDs)
    SET(${EXE_NAME}_LIBRARIES gdcmCommon gdcmDSED gdcmMEXD gdcmcharls gdcmjpeg12 gdcmjpeg8 gdcmuuid socketxx gdcmDICT gdcmIOD gdcmMSFF gdcmexpat gdcmjpeg16 gdcmopenjpeg gdcmzlib)

    add_executable(${EXE_NAME}   ${EXE_NAME}.cxx)
    target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})
ENDIF(${BUILD_ANONYMIZE})


