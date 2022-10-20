install(
    FILES
    mouse_rh.png
    mouse_lh.png
    DESTINATION
    ${KDE4_DATA_INSTALL_DIR}/kcminput/pics
)

if(NOT X11_Xcursor_FOUND)
    install(
        FILES
        arrow_large_black.png
        arrow_large_white.png
        arrow_small_black.png
        arrow_small_white.png
        DESTINATION
        ${KDE4_DATA_INSTALL_DIR}/kcminput/pics
    )
endif()
