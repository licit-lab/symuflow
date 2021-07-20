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

file(GLOB SYMUFLOW_SRC
    "*.h"
	  "*.hpp"
    "*.cpp"
    "*.c"
    "*/*.h"
	  "*/*.hpp"
    "*/*.cpp"
    "*/*/*.h"
    "*/*/*.cpp"
    "*/*/*/*.h"
    "*/*/*/*.cpp"
    "*.rc"
    "*.rgs"
)

# removing historical files not used anymore but keeped in the source code repository
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/CarrefourAFeux.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/CarrefourAFeux.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/StatUtil.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/StatUtil.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/XMLDocReseauCirculation.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/XMLDocReseauCirculation.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MemoryManagerImpl.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/MemoryManagerImpl.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Xerces/MemoryManagerImpl.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/Xerces/MemoryManagerImpl.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/SymuScriptManager.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/SymuScriptManager.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cryptage/tdes.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cryptage/tdes.c)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cryptage/tdesmain.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cryptage/Cryptage3DES.h)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/cryptage/Cryptage3DES.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/tests/Parallelisation/main.cpp)
list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/usage/SymuViaTripNodeDebug.cpp)

if(NOT USE_SYMUCOM)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/C_ITSApplication.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/C_ITSApplication.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/C_ITSScriptableApplication.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/C_ITSScriptableApplication.h)
	  list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/C_ITSExternalApplication.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/C_ITSExternalApplication.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/DerivedClass/GLOSA.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Applications/DerivedClass/GLOSA.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/ExternalLibraries/ITSExternalLibrary.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/ExternalLibraries/ITSExternalLibrary.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/BubbleSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/BubbleSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/GPSSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/GPSSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/InertialSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/InertialSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSDynamicSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSDynamicSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSScriptableDynamicSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSScriptableDynamicSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSScriptableStaticSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSScriptableStaticSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSExternalDynamicSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSExternalDynamicSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSExternalStaticSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSExternalStaticSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSStaticSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/ITSStaticSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/MagicSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/MagicSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/TelemetrySensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/TelemetrySensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/UBRSensor.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Sensors/DerivedClass/UBRSensor.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/ITSCAF.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/ITSCAF.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSExternalStation.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSExternalStation.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSScriptableStation.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSScriptableStation.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSStation.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSStation.h)
	  list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSStaticStation.cpp)
	  list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/ITSStaticStation.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/ITSVehicle.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/ITSVehicle.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/Simulator.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/Simulator.h)
	  list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/Simulator.h)
	  list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/SymuComSerialization.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/SymuComSerialization.h)
	  list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/ITSScriptableElement.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/ITSScriptableElement.h)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/BMAVehicle.cpp)
    list(REMOVE_ITEM SYMUFLOW_SRC ${CMAKE_CURRENT_SOURCE_DIR}/ITS/Stations/DerivedClass/BMAVehicle.h)
endif(NOT USE_SYMUCOM)


# Define SymuCore target: library
add_library(SymuFlow SHARED ${SYMUFLOW_SRC})

set_property(TARGET SymuFlow PROPERTY CXX_STANDARD 11)
set_property(TARGET SymuFlow PROPERTY CXX_STANDARD_REQUIRED)

# Link Library dependencies
# GDAL
target_link_libraries( SymuFlow ${GDAL_LIBRARY})

# SymuCore
target_link_libraries( SymuFlow SymuCore)

# Boost
# target_link_libraries( SymuFlow ${Boost_SERIALIZATION_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_WSERIALIZATION_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_PYTHON27_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_LOCALE_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_DATE_TIME_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_LOG_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_SYSTEM_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_FILESYSTEM_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_THREAD_LIBRARY})
# target_link_libraries( SymuFlow ${Boost_CHRONO_LIBRARY})
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


# file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/src/reseau.xsd DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

add_executable(SymuFlowLauncher launcher/SymuFlowLauncher.cpp)

set_property(TARGET SymuFlowLauncher PROPERTY CXX_STANDARD 11)
set_property(TARGET SymuFlowLauncher PROPERTY CXX_STANDARD_REQUIRED)

target_link_libraries(SymuFlowLauncher SymuFlow)
target_link_libraries(SymuFlowLauncher ${Boost_LIBRARIES})

install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/reseau.xsd DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS SymuFlow DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS SymuFlowLauncher DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
