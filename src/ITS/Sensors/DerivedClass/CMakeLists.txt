set(DERIVEDCLASS_HEADERS
  BubbleSensor.h
  GPSSensor.h
  InertialSensor.h
  MagicSensor.h
  TelemetrySensor.h
  UBRSensor.h)


set(DERIVEDCLASS_SRC
  BubbleSensor.cpp
  GPSSensor.cpp
  InertialSensor.cpp
  MagicSensor.cpp
  TelemetrySensor.cpp
  UBRSensor.cpp)

appendingfiles(SYMUFLOW_SRC DERIVEDCLASS_SRC)

install(FILES ${DERIVEDCLASS_HEADERS} DESTINATION include/SymuFlow/ITS/Sensors/DerivedClass)
