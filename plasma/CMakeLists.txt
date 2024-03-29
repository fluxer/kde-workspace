include_directories(
    # for kworkspace_export.h
    ${CMAKE_BINARY_DIR}/libs/kworkspace
)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1204)

MACRO(installPackage dir component)
   SET(root ${ARGN})
   IF(NOT root)
      SET(root plasma/plasmoids)
   ENDIF(NOT root)
   install(DIRECTORY ${dir}/ DESTINATION ${KDE4_DATA_INSTALL_DIR}/${root}/${component} PATTERN Messages.sh EXCLUDE PATTERN CMakeLists.txt EXCLUDE)
   install(FILES ${dir}/metadata.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR} RENAME plasma-applet-${component}.desktop)
ENDMACRO(installPackage)

add_subdirectory(applets)
add_subdirectory(containmentactions)
add_subdirectory(containments)
add_subdirectory(dataengines)
add_subdirectory(declarativeimports)
add_subdirectory(desktoptheme)
add_subdirectory(runners)
add_subdirectory(scriptengine)
add_subdirectory(shells)
add_subdirectory(wallpapers)
add_subdirectory(toolboxes)
add_subdirectory(sessions)
