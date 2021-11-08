add_subdirectory(DerivedClass)

set(SENSORS_HEADERS
  ITSDynamicSensor.h
  ITSExternalDynamicSensor.h
  ITSExternalStaticSensor.h
  ITSScriptableDynamicSensor.h
  ITSScriptableStaticSensor.h
  ITSSensor.h
  ITSStaticSensor.h)


set(SENSORS_SRC
  ITSDynamicSensor.cpp
  ITSExternalDynamicSensor.cpp
  ITSExternalStaticSensor.cpp
  ITSScriptableDynamicSensor.cpp
  ITSScriptableStaticSensor.cpp
  ITSSensor.cpp
  ITSStaticSensor.cpp)

appendingfiles(SYMUFLOW_SRC SENSORS_SRC)

install(FILES ${SENSORS_HEADERS} DESTINATION include/SymuFlow/ITS/Sensors)
