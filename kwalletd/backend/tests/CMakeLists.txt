include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/..
   ${CMAKE_CURRENT_BINARY_DIR}/..
)


########### next target ###############

set(backendtest_SRCS backendtest.cpp )


kde4_add_manual_test(backendtest ${backendtest_SRCS})

target_link_libraries(backendtest  ${KDE4_KDEUI_LIBS} kwalletbackend )


########### next target ###############

set(testbf_SRCS testbf.cpp )


kde4_add_manual_test(testbf ${testbf_SRCS})

target_link_libraries(testbf  ${KDE4_KDECORE_LIBS} kwalletbackend )

