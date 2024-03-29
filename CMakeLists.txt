cmake_minimum_required (VERSION 3.10)

project (SymuFlow VERSION 1.1.0)


list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
set(BOOST_PYTHONLIB python)


include("cmake/macro.cmake")

find_package(SymuCore REQUIRED)


find_package(Boost COMPONENTS
    chrono
    date_time
    locale
    log
    thread
    serialization
    wserialization
    filesystem
    system
    program_options
    ${BOOST_PYTHONLIB}
    REQUIRED)

    if(Boost_FOUND)
        message(STATUS "Library Boost is available")
        message(STATUS "\t ${Boost_CHRONO_LIBRARY}")
        message(STATUS "\t ${Boost_DATE_TIME_LIBRARY}")
        message(STATUS "\t ${Boost_LOCALE_LIBRARY}")
        message(STATUS "\t ${Boost_SERIALIZATION_LIBRARY}")
        message(STATUS "\t ${Boost_WSERIALIZATION_LIBRARY}")
        message(STATUS "\t ${Boost_PYTHON_LIBRARY}")
        message(STATUS "\t ${Boost_LOG_LIBRARY}")
        message(STATUS "\t ${Boost_FILESYSTEM_LIBRARY}")
        message(STATUS "\t ${Boost_THREAD_LIBRARY}")
        message(STATUS "\t ${Boost_PROGRAM_OPTIONS}")
        message(STATUS "\t ${Boost_SYSTEM_LIBRARY}")
        include_directories(${Boost_INCLUDE_DIRS})
        message(STATUS "\t Boost headers included")
    else()
        message(STATUS "\t Boost library is missing")
    endif()

# GDAL search
find_package(GDAL REQUIRED)

if(GDAL_FOUND)
    message(STATUS "Library GDAL is available")
    message(STATUS "\t GDAL library: ${GDAL_LIBRARY},\n \t Headers in: ${GDAL_INCLUDE_DIR}")
    include_directories(${GDAL_INCLUDE_DIR})
    message(STATUS "\t GDAL headers included")
else()
    message(STATUS "\t GDAL library is missing")
endif()


find_package(XercesC REQUIRED)

if(XercesC_FOUND)
    message(STATUS "Library XercesC is available")
    message(STATUS "\t XercesC library: ${XercesC_LIBRARIES},\n \t Headers in: ${XercesC_INCLUDE_DIRS}")
    include_directories(${XercesC_INCLUDE_DIRS})
    message(STATUS "\t XercesC headers included")
else()
    message(STATUS "\t XercesC library is missing")
endif()

# Python Search
find_package(Python3 3.9 COMPONENTS Interpreter Development REQUIRED)

if(Python3_FOUND)
    include_directories(${Python3_INCLUDE_DIRS})
    message(STATUS "Python is available")
    message(STATUS "\t Python library: ${Python3_LIBRARIES},\n \t Headers in: ${Python3_INCLUDE_DIRS}")
    message(STATUS "\t Python headers included")
else()
    message(STATUS "\t Python library is missing")
endif()


find_package(RapidJSON REQUIRED)

if(RapidJSON_FOUND)
    include_directories(${RapidJSON_INCLUDE_DIRS})
    message(STATUS "Rapidjson is available")
else()
  message(STATUS "\t Rapidjson library is missing")
endif()


find_package(Xqilla REQUIRED)

if(Xqilla_FOUND)
    include_directories(${XQILLA_INCLUDE_DIRS})
    message(STATUS "Xqilla is available")
    message(STATUS "\t Xqilla library: ${XQILLA_LIBRARIES},\n \t Headers in: ${XQILLA_INCLUDE_DIRS}")
else()
    message(STATUS "\t Xqilla library is missing")
    message(STATUS "\t Xqilla library: ${Xqilla_LIBRARIES},\n \t Headers in: ${Xqilla_INCLUDE_DIRS}")
endif()


# Define libraries to link
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


add_subdirectory(src)


# Install the FindXqilla helper
install(
  FILES
  ${CMAKE_SOURCE_DIR}/cmake/modules/FindXqilla.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/symuflow)

install(EXPORT SymuFlowTarget DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/symuflow)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(${CMAKE_BINARY_DIR}/SymuFlow/SymuFlowConfigVersion.cmake
  VERSION ${SymuFlow_VERSION}
  COMPATIBILITY ExactVersion)

configure_package_config_file(${PROJECT_SOURCE_DIR}/cmake/SymuFlowConfig.cmake.in
  ${CMAKE_BINARY_DIR}/SymuFlow/SymuFlowConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/symuflow)

install(
  FILES
  ${CMAKE_BINARY_DIR}/SymuFlow/SymuFlowConfig.cmake
  ${CMAKE_BINARY_DIR}/SymuFlow/SymuFlowConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/cmake/symuflow
  COMPONENT Development)
