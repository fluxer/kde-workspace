project(plasma-python)

# install the library, .desktop, and plasma.py
install(
    FILES
    pyappletscript.py
    plasma_importer.py
    pydataengine.py
    pyrunner.py
    pywallpaper.py
    DESTINATION
    ${DATA_INSTALL_DIR}/plasma_scriptengine_python
)
install(
    FILES plasmascript.py
    DESTINATION ${PYTHON_SITE_PACKAGES_DIR}/PyKDE4
)

install(
    FILES
    plasma-scriptengine-applet-python.desktop
    plasma-scriptengine-dataengine-python.desktop
    plasma-scriptengine-runner-python.desktop
    plasma-scriptengine-wallpaper-python.desktop
    DESTINATION ${SERVICES_INSTALL_DIR}
)

###########################################################################
# Test plasmoids
option(INSTALL_PYTHON_TEST_PLASMA "Install test python applet" OFF)

if(INSTALL_PYTHON_TEST_PLASMA )

    install(DIRECTORY test/plasma-applet-pyclock DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids)

    #TODO where install it ?
    #install(DIRECTORY test/plasma-dataengine-pytime DESTINATION ${DATA_INSTALL_DIR}/plasma/engines)

endif(INSTALL_PYTHON_TEST_PLASMA)
