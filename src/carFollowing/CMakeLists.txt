set(CARFOLLOWING_HEADERS
  AbstractCarFollowing.h
  CarFollowingContext.h
  CarFollowingFactory.h
  GippsCarFollowing.h
  IDMCarFollowing.h
  NewellCarFollowing.h
  NewellContext.h
  ScriptedCarFollowing.h)


set(CARFOLLOWING_SRC
  AbstractCarFollowing.cpp
  CarFollowingContext.cpp
  CarFollowingFactory.cpp
  GippsCarFollowing.cpp
  IDMCarFollowing.cpp
  NewellCarFollowing.cpp
  NewellContext.cpp
  ScriptedCarFollowing.cpp)

appendingfiles(SYMUFLOW_SRC CARFOLLOWING_SRC)

install(FILES ${CARFOLLOWING_HEADERS} DESTINATION include/SymuFlow/carFollowing)
