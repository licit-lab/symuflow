set(REGULATION_HEADERS
  PythonUtils.h
  RegulationAction.h
  RegulationBrique.h
  RegulationCapteur.h
  RegulationCondition.h
  RegulationElement.h
  RegulationModule.h
  RegulationRestitution.h)


set(REGULATION_SRC
  PythonUtils.cpp
  RegulationAction.cpp
  RegulationBrique.cpp
  RegulationCapteur.cpp
  RegulationCondition.cpp
  RegulationElement.cpp
  RegulationModule.cpp
  RegulationRestitution.cpp)

appendingfiles(SYMUFLOW_SRC REGULATION_SRC)

install(FILES ${REGULATION_HEADERS} DESTINATION include/SymuFlow/regulation)
