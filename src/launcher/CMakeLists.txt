add_executable(SymuFlowLauncher SymuFlowLauncher.cpp)

set_property(TARGET SymuFlowLauncher PROPERTY CXX_STANDARD 11)
set_property(TARGET SymuFlowLauncher PROPERTY CXX_STANDARD_REQUIRED)

target_link_libraries(SymuFlowLauncher SymuFlow)
target_link_libraries(SymuFlowLauncher ${Boost_LIBRARIES})

install(TARGETS SymuFlowLauncher DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
