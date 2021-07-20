set(EXTLIBS_HEADERS
  ITSExternalLibrary.h)

set(EXTLIBS_SRC
  ITSExternalLibrary.cpp)

appendingfiles(SYMUFLOW_SRC EXTLIBS_SRC)

install(FILES ${EXTLIBS_HEADERS} DESTINATION include/SymuFlow/ITS/ExternalLibraries)
