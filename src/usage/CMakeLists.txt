add_subdirectory(logistic)

set(USAGE_HEADERS
  AbstractFleet.h
  AbstractFleetParameters.h
  LoadUsageParameters.h
  Passenger.h
  Position.h
  PublicTransportFleet.h
  PublicTransportFleetParameters.h
  PublicTransportLine.h
  PublicTransportScheduleParameters.h
  Schedule.h
  ScheduleParameters.h
  SymuViaFleet.h
  SymuViaFleetParameters.h
  SymuViaTripNode.h
  SymuViaVehicleToCreate.h
  Trip.h
  TripLeg.h
  TripNode.h
  VehicleToCreate.h
  VehicleTypeUsageParameters.h)


set(USAGE_SRC
  AbstractFleet.cpp
  AbstractFleetParameters.cpp
  LoadUsageParameters.cpp
  Passenger.cpp
  Position.cpp
  PublicTransportFleet.cpp
  PublicTransportFleetParameters.cpp
  PublicTransportLine.cpp
  PublicTransportScheduleParameters.cpp
  Schedule.cpp
  ScheduleParameters.cpp
  SymuViaFleet.cpp
  SymuViaFleetParameters.cpp
  SymuViaTripNode.cpp
  SymuViaVehicleToCreate.cpp
  Trip.cpp
  TripLeg.cpp
  TripNode.cpp
  VehicleToCreate.cpp
  VehicleTypeUsageParameters.cpp)

appendingfiles(SYMUFLOW_SRC USAGE_SRC)

install(FILES ${USAGE_HEADERS} DESTINATION include/SymuFlow/usage)
