add_subdirectory(Applications)
add_subdirectory(ExternalLibraries)
add_subdirectory(Sensors)
add_subdirectory(Stations)

set(ITS_HEADERS
  ITSScriptableElement.h
  SymuComSerialization.h)


set(ITS_SRC
  ITSScriptableElement.cpp
  SymuComSerialization.cpp)

appendingfiles(SYMUFLOW_SRC ITS_SRC)

install(FILES ${ITS_HEADERS} DESTINATION include/SymuFlow/ITS)
