macro_optional_add_subdirectory(ruby)

if(QT_QTWEBKIT_FOUND)
    macro_optional_add_subdirectory(webkit)
endif()

if (PYTHONLIBS_FOUND)
    macro_optional_add_subdirectory(python)
endif()

macro_optional_add_subdirectory(javascript)