project(kcm-touchpad)

set(TOUCHPAD_KCM_VERSION "1.1")

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

include(KDE4Defaults)
include_directories(${KDE4_INCLUDES})

add_subdirectory(src)
add_subdirectory(icon)
