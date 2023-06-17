# KHotKey convenience library. Contains
# - Actions
# - Trigger
# - Conditions
# - Some gui stuff

set(khotkeysprivate_SRCS

    # ACTIONS
    actions/action.cpp
    actions/action_list.cpp
    actions/activate_window_action.cpp
    actions/command_url_action.cpp
    actions/dbus_action.cpp
    actions/keyboard_input_action.cpp
    actions/menuentry_action.cpp

    # ACTION DATA
    action_data/action_data.cpp
    action_data/action_data_base.cpp
    action_data/action_data_group.cpp
    action_data/generic_action_data.cpp
    action_data/menuentry_shortcut_action_data.cpp
    action_data/simple_action_data.cpp

    action_data/action_data_visitor.cpp

    # CONDITIONS
    conditions/active_window_condition.cpp
    conditions/and_condition.cpp
    conditions/condition.cpp
    conditions/conditions_list_base.cpp
    conditions/conditions_list.cpp
    conditions/existing_window_condition.cpp
    conditions/not_condition.cpp
    conditions/or_condition.cpp
    conditions/conditions_visitor.cpp

    # TRIGGERS
    triggers/shortcut_trigger.cpp
    triggers/trigger.cpp
    triggers/trigger_list.cpp
    triggers/triggers.cpp
    triggers/window_trigger.cpp

    # WINDOWS_HELPER
    windows_helper/window_selection_rules.cpp
    windows_helper/window_selection_list.cpp
    windows_helper/window_selection_interface.cpp

    # DAEMON
    daemon/daemon.h
    daemon/daemon.cpp

    # OTHER
    khotkeysglobal.cpp
    settings.cpp
    settings_reader_v2.cpp
    settings_writer.cpp
    shortcuts_handler.cpp
    windows_handler.cpp
)

add_library(khotkeysprivate SHARED ${khotkeysprivate_SRCS})
target_link_libraries(khotkeysprivate
    KDE4::kio
    ${X11_LIBRARIES}
    kworkspace
)
if(X11_XTest_FOUND)
    target_link_libraries(khotkeysprivate ${X11_XTest_LIB})
endif(X11_XTest_FOUND)
set_target_properties(khotkeysprivate PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(
    TARGETS khotkeysprivate
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
    LIBRARY NAMELINK_SKIP
)

