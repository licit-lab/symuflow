add_subdirectory(DerivedClass)

set(APPLICATION_HEADERS
  C_ITSApplication.h
  C_ITSExternalApplication.h
  C_ITSScriptableApplication.h)


set(APPLICATION_SRC
  C_ITSApplication.cpp
  C_ITSExternalApplication.cpp
  C_ITSScriptableApplication.cpp)

appendingfiles(SYMUFLOW_SRC APPLICATION_SRC)

install(FILES ${APPLICATION_HEADERS} DESTINATION include/SymuFlow/ITS/Applications)
