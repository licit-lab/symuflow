set(LOGISTIC_HEADERS
  DeliveryFleet.h
  DeliveryFleetParameters.h
  PointDeLivraison.h)


set(LOGISTIC_SRC
  DeliveryFleet.cpp
  DeliveryFleetParameters.cpp
  PointDeLivraison.cpp)

appendingfiles(SYMUFLOW_SRC LOGISTIC_SRC)

install(FILES ${LOGISTIC_HEADERS} DESTINATION include/SymuFlow/usage/logistic)
