add_subdirectory(DerivedClass)

set(STATIONS_HEADERS
  ITSExternalStation.h
  ITSScriptableStation.h
  ITSStation.h)


set(STATIONS_SRC
  ITSExternalStation.cpp
  ITSScriptableStation.cpp
  ITSStation.cpp)

appendingfiles(SYMUFLOW_SRC STATIONS_SRC)

install(FILES ${STATIONS_HEADERS} DESTINATION include/SymuFlow/ITS/Stations)
