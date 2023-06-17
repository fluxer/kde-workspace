include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../
${CMAKE_CURRENT_SOURCE_DIR}/../processcore/
${CMAKE_CURRENT_SOURCE_DIR}/../processui/ )

# Process unit test
kde4_add_test(ksysguard-processtest processtest.cpp)
target_link_libraries(ksysguard-processtest processui KDE4::kdecore ${QT_QTTEST_LIBRARY})

# KSignalPlotter benchmark
set(signalplotterbenchmark_SRCS signalplotterbenchmark.cpp ../signalplotter/ksignalplotter.cpp)
kde4_add_test(ksysguard-signalplotterbenchmark ${signalplotterbenchmark_SRCS})
target_link_libraries(ksysguard-signalplotterbenchmark KDE4::kdeui ${QT_QTTEST_LIBRARY} ${QT_QTBENCHMARK_LIBRARY})

# KSignalPlotter unit test
set( signalplottertest_SRCS signalplottertest.cpp ../signalplotter/ksignalplotter.cpp)
kde4_add_test(ksysguard-signalplottertest ${signalplottertest_SRCS} )
target_link_libraries(ksysguard-signalplottertest KDE4::kdeui ${QT_QTTEST_LIBRARY})
