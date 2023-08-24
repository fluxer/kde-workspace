include(CheckIncludeFiles)

### BSD util library
find_library(UTIL_LIBRARY util)
mark_as_advanced(UTIL_LIBRARY)

### NetBSD kvm library
find_library(KVM_LIBRARY kvm)
mark_as_advanced(KVM_LIBRARY)

### Tests
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

### Font Embedder and LineFont.h
option(KONSOLE_BUILD_FONTEMBEDDER "Konsole: build fontembedder executable" OFF)

### Use cmake -DKONSOLE_BUILD_FONTEMBEDDER
if(KONSOLE_BUILD_FONTEMBEDDER)
    ### Font Embedder
    add_executable(fontembedder fontembedder.cpp)
    target_link_libraries(fontembedder KDE4::kio)

    ### Line graphics font
    ### The below cmake doesn't work; it might be possible to automate it
    ###   if LineFont.src has changed.  If automated, different OSes will
    ###   need to be handled.
    ###   If LineFont.h is needed to be recreated use:
    ###     fontembedder LineFont.src > LineFont.h
    #add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/LineFont.h
    #                   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/fontembedder
    #                   ARGS ${CMAKE_SOURCE_DIR}/LineFont.src
    #                   DEPENDS ${CMAKE_SOURCE_DIR}/LineFont.src )
endif()

### Konsole source files shared between embedded terminal and main application
# qdbuscpp2xml -m  Session.h -o org.kde.konsole.Session.xml
# qdbuscpp2xml -M -s ViewManager.h -o org.kde.konsole.Konsole.xml

# Generate dbus .xml files; do not store .xml in source folder
qt4_generate_dbus_interface(
    Session.h
    org.kde.konsole.Session.xml
    OPTIONS -M
)
qt4_generate_dbus_interface(
    ViewManager.h
    org.kde.konsole.Window.xml
    OPTIONS -M
)

qt4_add_dbus_adaptor(sessionadaptors_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.konsole.Session.xml
    Session.h
    Konsole::Session
)
qt4_add_dbus_adaptor(windowadaptors_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.konsole.Window.xml
    ViewManager.h
    Konsole::ViewManager
)

set(konsoleprivate_SRCS
    ${sessionadaptors_SRCS}
    ${windowadaptors_SRCS}
    BookmarkHandler.cpp
    ColorScheme.cpp
    ColorSchemeManager.cpp
    ColorSchemeEditor.cpp
    CopyInputDialog.cpp
    EditProfileDialog.cpp
    Emulation.cpp
    Filter.cpp
    History.cpp
    HistorySizeDialog.cpp
    HistorySizeWidget.cpp
    IncrementalSearchBar.cpp
    KeyBindingEditor.cpp
    KeyboardTranslator.cpp
    KeyboardTranslatorManager.cpp
    ManageProfilesDialog.cpp
    ProcessInfo.cpp
    Profile.cpp
    ProfileList.cpp
    ProfileReader.cpp
    ProfileWriter.cpp
    ProfileManager.cpp
    Pty.cpp
    RenameTabDialog.cpp
    RenameTabWidget.cpp
    Screen.cpp
    ScreenWindow.cpp
    Session.cpp
    SessionController.cpp
    SessionManager.cpp
    SessionListModel.cpp
    ShellCommand.cpp
    TabTitleFormatButton.cpp
    TerminalCharacterDecoder.cpp
    ExtendedCharTable.cpp
    TerminalDisplay.cpp
    ViewContainer.cpp
    ViewContainerTabBar.cpp
    ViewManager.cpp
    ViewProperties.cpp
    ViewSplitter.cpp
    Vt102Emulation.cpp
    ZModemDialog.cpp
    PrintOptions.cpp
    konsole_wcwidth.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.konsole.Window.xml
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.konsole.Session.xml
)

### Konsole Application

set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/TerminalDisplay.cpp
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE
)

add_library(konsoleprivate SHARED ${konsoleprivate_SRCS})

target_link_libraries(konsoleprivate
    KDE4::kpty
    KDE4::kio
    KDE4::kdeui
    ${QT_QTXML_LIBRARY}
    konq
)

if(UTIL_LIBRARY)
    target_link_libraries(konsoleprivate
        ${UTIL_LIBRARY}
    )
endif()

if(KVM_LIBRARY)
    target_link_libraries(konsoleprivate
        ${KVM_LIBRARY}
    )
endif()

set(konsole_SRCS
    Application.cpp
    MainWindow.cpp
    main.cpp
    settings/GeneralSettings.cpp
    settings/TabBarSettings.cpp
)

kde4_add_kcfg_files(konsole_SRCS
    settings/KonsoleSettings.kcfgc
)

add_executable(konsole ${konsole_SRCS})

target_link_libraries(konsole PRIVATE konsoleprivate)

generate_export_header(konsoleprivate)

install(
    TARGETS konsole
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

install(
    TARGETS konsoleprivate
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

### Embedded Konsole KPart

kde4_add_plugin(konsolepart Part.cpp)
target_link_libraries(konsolepart
    KDE4::kparts
    konsoleprivate
)
install(
    TARGETS konsolepart
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

### konsoleprofile command-line tool

install(
    PROGRAMS konsoleprofile
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
