set(SENSORS_HEADERS
  AbstractSensor.h
  BlueToothSensor.h
  EdieSensor.h
  GlobalSensor.h
  LongitudinalSensor.h
  MFDSensor.h
  PonctualSensor.h
  SensorsManager.h
  TankSensor.h)


set(SENSORS_SRC
  AbstractSensor.cpp
  BlueToothSensor.cpp
  EdieSensor.cpp
  GlobalSensor.cpp
  LongitudinalSensor.cpp
  MFDSensor.cpp
  PonctualSensor.cpp
  SensorsManager.cpp
  TankSensor.cpp)

appendingfiles(SYMUFLOW_SRC SENSORS_SRC)

install(FILES ${SENSORS_HEADERS} DESTINATION include/SymuFlow/sensors)
