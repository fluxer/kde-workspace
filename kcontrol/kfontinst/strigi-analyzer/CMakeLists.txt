include_directories(
    ${CMAKE_SOURCE_DIR}/kcontrol/kfontinst/lib/
    ${STRIGI_INCLUDE_DIR}
)

add_library(font MODULE FontThroughAnalyzer.cpp FontEngine.cpp)

target_link_libraries(font
    ${STRIGI_STREAMANALYZER_LIBRARY}
    ${FREETYPE_LIBRARIES}
    ${FONTCONFIG_LIBRARIES}
    ${KDE4_KIO_LIBS}
    kfontinst
)

set_target_properties(font PROPERTIES  PREFIX strigi_)

install(TARGETS font LIBRARY DESTINATION ${KDE4_LIB_INSTALL_DIR}/strigi)

