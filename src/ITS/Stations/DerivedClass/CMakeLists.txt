set(DERIVEDCLASS_HEADERS
  BMAVehicle.h
  ITSCAF.h
  ITSVehicle.h
  Simulator.h)


set(DERIVEDCLASS_SRC
  BMAVehicle.cpp
  ITSCAF.cpp
  ITSVehicle.cpp
  Simulator.cpp)

appendingfiles(SYMUFLOW_SRC DERIVEDCLASS_SRC)

install(FILES ${DERIVEDCLASS_HEADERS} DESTINATION include/SymuFlow/DERIVEDCLASS)
