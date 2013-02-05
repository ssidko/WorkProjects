# Install script for directory: D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/test/unit

# Set the install prefix
IF(NOT DEFINED CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "C:/Program Files/MySQL/ConnectorCPP")
ENDIF(NOT DEFINED CMAKE_INSTALL_PREFIX)
STRING(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
IF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  IF(BUILD_TYPE)
    STRING(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  ELSE(BUILD_TYPE)
    SET(CMAKE_INSTALL_CONFIG_NAME "Release")
  ENDIF(BUILD_TYPE)
  MESSAGE(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
ENDIF(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)

# Set the component getting installed.
IF(NOT CMAKE_INSTALL_COMPONENT)
  IF(COMPONENT)
    MESSAGE(STATUS "Install component: \"${COMPONENT}\"")
    SET(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  ELSE(COMPONENT)
    SET(CMAKE_INSTALL_COMPONENT)
  ENDIF(COMPONENT)
ENDIF(NOT CMAKE_INSTALL_COMPONENT)

IF(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  INCLUDE("D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/test/unit/example/cmake_install.cmake")
  INCLUDE("D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/test/unit/classes/cmake_install.cmake")
  INCLUDE("D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/test/unit/performance/cmake_install.cmake")
  INCLUDE("D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/test/unit/bugs/cmake_install.cmake")
  INCLUDE("D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/test/unit/template_bug_group/cmake_install.cmake")

ENDIF(NOT CMAKE_INSTALL_LOCAL_ONLY)

