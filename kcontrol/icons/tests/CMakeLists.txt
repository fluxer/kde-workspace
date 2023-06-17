include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)

########### next target ###############

set(testicons_SRCS
    testicons.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../icons.cpp
)

kde4_add_manual_test(testicons ${testicons_SRCS})

target_link_libraries(testicons KDE4::kdeui)
