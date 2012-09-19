# Install script for directory: D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn

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

IF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")
  FILE(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/cppconn" TYPE FILE FILES
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/build_config.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/config.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/connection.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/datatype.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/driver.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/exception.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/metadata.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/parameter_metadata.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/prepared_statement.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/resultset.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/resultset_metadata.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/statement.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/sqlstring.h"
    "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/cppconn/warning.h"
    )
ENDIF(NOT CMAKE_INSTALL_COMPONENT OR "${CMAKE_INSTALL_COMPONENT}" MATCHES "^(Unspecified)$")

