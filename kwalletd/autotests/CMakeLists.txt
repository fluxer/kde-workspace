add_definitions(-DTEST_DATA="\\\"${CMAKE_CURRENT_SOURCE_DIR}/\\\"")

set(testname testpamopen)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_test(kwalletd-${testname} kwalletexecuter.cpp ${testname}.cpp)
target_link_libraries(kwalletd-${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBS})

set(testname testpamopennofile)
include_directories(${QT_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR})
kde4_add_test(kwalletd-${testname} kwalletexecuter.cpp ${testname}.cpp)
target_link_libraries(kwalletd-${testname} ${QT_QTTEST_LIBRARY} ${QT_QTCORE_LIBRARY} ${QT_QTNETWORK_LIBRARY} ${KDE4_KDECORE_LIBS})