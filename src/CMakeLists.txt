set(USE_SYMUCOM OFF CACHE BOOL "Use SymuCom")

# Version control
set(SYMUVIA_VERSION_MAJOR 9)
set(SYMUVIA_VERSION_MINOR 0)
set(SYMUVIA_VERSION_REVISION 0)
set(SYMUVIA_VERSION_BUILD 0)


# Not clear what this is for.
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/SymuviaVersion.h.in
  ${CMAKE_CURRENT_SOURCE_DIR}/SymuviaVersion.h
  @ONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/Symubruit.rc.in
  ${CMAKE_CURRENT_SOURCE_DIR}/Symubruit.rc
  @ONLY)

# Set processor flags
add_definitions(-DTRAFFICSTATE_EXPORTS)
add_definitions(-DSYMUVIA_EXPORTS)
add_definitions(-DUSE_SYMUCORE)
add_definitions(-DBOOST_LOG_DYN_LINK)

if(USE_SYMUCOM)
    add_definitions(-DUSE_SYMUCOM)
endif(USE_SYMUCOM)

# file(GLOB SYMUFLOW_SRC
#     "*.h"
# 	  "*.hpp"
#     "*.cpp"
#     "*.c"
#     "*/*.h"
# 	  "*/*.hpp"
#     "*/*.cpp"
#     "*/*/*.h"
#     "*/*/*.cpp"
#     "*/*/*/*.h"
#     "*/*/*/*.cpp"
#     "*.rc"
#     "*.rgs"
# )

set_property(GLOBAL PROPERTY SYMUFLOW_SRC "")

set(ROOT_HEADERS
  Affectation.h
  AffectationSave.h
  ArretPublicTransportPenalty.h
  BriqueDeConnexion.h
  CSVOutputWriter.h
  CarrefourAFeuxEx.h
  Connection.h
  ConnectionDrivingPenalty.h
  ConnectionPonctuel.h
  ControlZone.h
  ControleurDeFeux.h
  DLL_TrafficState.h
  DiagFonda.h
  DocAcoustique.h
  DocTrafic.h
  EVEDocAcoustique.h
  EVEDocTrafic.h
  GMLDocTrafic.h
  GTFSExporter.h
  JSONDocTrafic.h
  Logger.h
  MergingObject.h
  MesoNode.h
  Motif.h
  ParcRelaisPenalty.h
  Parking.h
  ParkingParameters.h
  Plaque.h
  PlaqueDrivingPattern.h
  RandManager.h
  ReaderWriterLock.h
  RepMotif.h
  RepartitionTypeVehicule.h
  SQLNetworkExporter.h
  SQLSymuvia.h
  Segment.h
  SerializeUtil.h
  SimulationSnapshot.h
  StaticElement.h
  SymExp.h
  SymExpC.h
  SymuCoreAreaHelper.h
  SymuCoreDrivingGraphBuilder.h
  SymuCoreDrivingTripsBuilder.h
  SymuCoreGraphBuilder.h
  SymuCoreManager.h
  SymuCorePublicTransportGraphBuilder.h
  SymuViaDrivingPattern.h
  SymuViaPublicTransportPattern.h
  Symubruit.h
  SymubruitExports.h
  SymuviaVersion.h
  SymuviaVersion.h.in
  SystemUtil.h
  TempsCritique.h
  TimeUtil.h
  TraceDocAcoustique.h
  TraceDocTrafic.h
  TrafficState.h
  TravelTimesOutputManager.h
  TronconDestination.h
  TronconDrivingPattern.h
  TronconOrigine.h
  TuyauMeso.h
  VehicleTypePenalty.h
  XMLDocAcoustique.h
  XMLDocEmissionsAtmo.h
  XMLDocItineraire.h
  XMLDocSirane.h
  XMLDocTrafic.h
  XMLReaderTrafic.h
  ZoneDeTerminaison.h
  ZoneDrivingPattern.h
  arret.h
  convergent.h
  divergent.h
  entree.h
  feux.h
  frontiere.h
  giratoire.h
  loi_emission.h
  repartiteur.h
  reseau.h
  resource.h
  sortie.h
  stdafx.h
  symUtil.h
  tools.h
  troncon.h
  tuyau.h
  vehicule.h
  voie.h)


set(ROOT_SRC
  Affectation.cpp
  AffectationSave.cpp
  ArretPublicTransportPenalty.cpp
  BriqueDeConnexion.cpp
  CSVOutputWriter.cpp
  CarrefourAFeuxEx.cpp
  Connection.cpp
  ConnectionDrivingPenalty.cpp
  ConnectionPonctuel.cpp
  ControlZone.cpp
  ControleurDeFeux.cpp
  DiagFonda.cpp
  DocAcoustique.cpp
  DocTrafic.cpp
  EVEDocAcoustique.cpp
  EVEDocTrafic.cpp
  GMLDocTrafic.cpp
  GTFSExporter.cpp
  JSONDocTrafic.cpp
  Logger.cpp
  MergingObject.cpp
  MesoNode.cpp
  Motif.cpp
  ParcRelaisPenalty.cpp
  Parking.cpp
  ParkingParameters.cpp
  Plaque.cpp
  PlaqueDrivingPattern.cpp
  RandManager.cpp
  ReaderWriterLock.cpp
  RepMotif.cpp
  RepartitionTypeVehicule.cpp
  SQLNetworkExporter.cpp
  SQLSymuvia.cpp
  Segment.cpp
  SerializeUtil.cpp
  SimulationSnapshot.cpp
  StaticElement.cpp
  SymuCoreAreaHelper.cpp
  SymuCoreDrivingGraphBuilder.cpp
  SymuCoreDrivingTripsBuilder.cpp
  SymuCoreGraphBuilder.cpp
  SymuCoreManager.cpp
  SymuCorePublicTransportGraphBuilder.cpp
  SymuViaDrivingPattern.cpp
  SymuViaPublicTransportPattern.cpp
  Symubruit.cpp
  SystemUtil.cpp
  TempsCritique.cpp
  TimeUtil.cpp
  TraceDocAcoustique.cpp
  TraceDocTrafic.cpp
  TrafficState.cpp
  TravelTimesOutputManager.cpp
  TronconDestination.cpp
  TronconDrivingPattern.cpp
  TronconOrigine.cpp
  TuyauMeso.cpp
  VehicleTypePenalty.cpp
  XMLDocAcoustique.cpp
  XMLDocEmissionsAtmo.cpp
  XMLDocItineraire.cpp
  XMLDocSirane.cpp
  XMLDocTrafic.cpp
  XMLReaderTrafic.cpp
  ZoneDeTerminaison.cpp
  ZoneDrivingPattern.cpp
  arret.cpp
  convergent.cpp
  divergent.cpp
  entree.cpp
  feux.cpp
  frontiere.cpp
  giratoire.cpp
  loi_emission.cpp
  repartiteur.cpp
  reseau.cpp
  sortie.cpp
  stdafx.cpp
  symUtil.cpp
  tools.cpp
  troncon.cpp
  tuyau.cpp
  vehicule.cpp
  voie.cpp)

appendingfiles(SYMUFLOW_SRC ROOT_SRC)

install(FILES ${ROOT_HEADERS} DESTINATION include/SymuFlow)

add_subdirectory(Xerces)
add_subdirectory(usage)
add_subdirectory(sensors)
add_subdirectory(regulation)
add_subdirectory(launcher)
add_subdirectory(config)
add_subdirectory(carFollowing)


get_property(SYMUFLOW_SRC GLOBAL PROPERTY SYMUFLOW_SRC)


if(USE_SYMUCOM)
  add_subdirectory(ITS)
endif(USE_SYMUCOM)


# Define SymuCore target: library
add_library(SymuFlow SHARED ${SYMUFLOW_SRC})

set_property(TARGET SymuFlow PROPERTY CXX_STANDARD 11)
set_property(TARGET SymuFlow PROPERTY CXX_STANDARD_REQUIRED)

# Link Library dependencies
# GDAL
target_link_libraries( SymuFlow ${GDAL_LIBRARY})

# SymuCore
target_link_libraries( SymuFlow SymuCore)


target_link_libraries( SymuFlow ${Boost_LIBRARIES})


# XQilla
target_link_libraries( SymuFlow ${XQILLA_LIBRARIES})

# Xerces-c
target_link_libraries( SymuFlow ${XercesC_LIBRARIES})

# Rapidjson
#target_link_libraries( SymuFlow ${RAPIDJSON_LIBRARY}/${RAPIDJSON_LIBRARY})

# Python
target_link_libraries( SymuFlow ${Python3_LIBRARIES})

# Visible
target_include_directories(SymuFlow PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:include/SymuFlow>
)


install(TARGETS SymuFlow DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
