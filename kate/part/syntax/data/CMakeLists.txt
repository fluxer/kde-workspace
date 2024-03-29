########### install files ###############

add_custom_target(GeneratePhpXmlFiles
    DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/css-php.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/html-php.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/javascript-php.xml
)


macro(GENERATE_PHP_XML targetFile srcFile)
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/${targetFile}
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/generate-php.pl < ${CMAKE_CURRENT_SOURCE_DIR}/${srcFile} > ${CMAKE_CURRENT_SOURCE_DIR}/${targetFile}
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${srcFile}
    )
endmacro(GENERATE_PHP_XML)

generate_php_xml(javascript-php.xml javascript.xml)
generate_php_xml(css-php.xml css.xml)
generate_php_xml(html-php.xml html.xml)

file(GLOB HIGHLIGHTERS "${CMAKE_CURRENT_SOURCE_DIR}/*.xml")

install(
    FILES
    language.dtd syntax.template
    ${HIGHLIGHTERS}
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/katepart/syntax
)
