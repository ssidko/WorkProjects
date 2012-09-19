# This file will be configured to contain variables for CPack. These variables
# should be set in the CMake list file of the project before CPack module is
# included. Example variables are:
#   CPACK_GENERATOR                     - Generator used to create package
#   CPACK_INSTALL_CMAKE_PROJECTS        - For each project (path, name, component)
#   CPACK_CMAKE_GENERATOR               - CMake Generator used for the projects
#   CPACK_INSTALL_COMMANDS              - Extra commands to install components
#   CPACK_INSTALL_DIRECTORIES           - Extra directories to install
#   CPACK_PACKAGE_DESCRIPTION_FILE      - Description file for the package
#   CPACK_PACKAGE_DESCRIPTION_SUMMARY   - Summary of the package
#   CPACK_PACKAGE_EXECUTABLES           - List of pairs of executables and labels
#   CPACK_PACKAGE_FILE_NAME             - Name of the package generated
#   CPACK_PACKAGE_ICON                  - Icon used for the package
#   CPACK_PACKAGE_INSTALL_DIRECTORY     - Name of directory for the installer
#   CPACK_PACKAGE_NAME                  - Package project name
#   CPACK_PACKAGE_VENDOR                - Package project vendor
#   CPACK_PACKAGE_VERSION               - Package project version
#   CPACK_PACKAGE_VERSION_MAJOR         - Package project version (major)
#   CPACK_PACKAGE_VERSION_MINOR         - Package project version (minor)
#   CPACK_PACKAGE_VERSION_PATCH         - Package project version (patch)

# There are certain generator specific ones

# NSIS Generator:
#   CPACK_PACKAGE_INSTALL_REGISTRY_KEY  - Name of the registry key for the installer
#   CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS - Extra commands used during uninstall
#   CPACK_NSIS_EXTRA_INSTALL_COMMANDS   - Extra commands used during install


SET(CPACK_BINARY_BUNDLE "")
SET(CPACK_BINARY_CYGWIN "")
SET(CPACK_BINARY_DEB "")
SET(CPACK_BINARY_NSIS "")
SET(CPACK_BINARY_OSXX11 "")
SET(CPACK_BINARY_PACKAGEMAKER "")
SET(CPACK_BINARY_RPM "")
SET(CPACK_BINARY_STGZ "")
SET(CPACK_BINARY_TBZ2 "")
SET(CPACK_BINARY_TGZ "")
SET(CPACK_BINARY_TZ "")
SET(CPACK_BINARY_ZIP "")
SET(CPACK_CMAKE_GENERATOR "Visual Studio 9 2008")
SET(CPACK_COMPONENT_UNSPECIFIED_HIDDEN "TRUE")
SET(CPACK_COMPONENT_UNSPECIFIED_REQUIRED "TRUE")
SET(CPACK_GENERATOR "ZIP")
SET(CPACK_INSTALL_CMAKE_PROJECTS "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0;MYSQLCPPCONN;ALL;/")
SET(CPACK_INSTALL_PREFIX "C:/Program Files/MYSQLCPPCONN")
SET(CPACK_MODULE_PATH "")
SET(CPACK_NSIS_DISPLAY_NAME "mysql-connector-c++-1.1.0-win32")
SET(CPACK_NSIS_INSTALLER_ICON_CODE "")
SET(CPACK_NSIS_INSTALLER_MUI_ICON_CODE "")
SET(CPACK_OUTPUT_CONFIG_FILE "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/CPackConfig.cmake")
SET(CPACK_PACKAGE_DEFAULT_LOCATION "/")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "C:/Program Files/CMake 2.6/share/cmake-2.6/Templates/CPack.GenericDescription.txt")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MYSQLCPPCONN built using CMake")
SET(CPACK_PACKAGE_FILE_NAME "mysql-connector-c++-noinstall-1.1.0-win32")
SET(CPACK_PACKAGE_IGNORE_FILES "/CMakeFiles/;/Testing/;/.bzr/;_CPack_Packages/;.cmake$;~;.swp;.log;.gz;.directory$;CMakeCache.txt;Makefile;install_manifest.txt;ANNOUNCEMENT_102_ALPHA;ANNOUNCEMENT_103_ALPHA;ANNOUNCEMENT_104_BETA;ANNOUNCEMENT_105_GA;ANNOUNCEMENT_110_GA;ANNOUNCEMENT_DRAFT;something_there")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "mysql-connector-c++-1.1.0-win32")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "mysql-connector-c++ 1.1.0")
SET(CPACK_PACKAGE_NAME "mysql-connector-c++")
SET(CPACK_PACKAGE_RELEASE_TYPE "")
SET(CPACK_PACKAGE_RELOCATABLE "true")
SET(CPACK_PACKAGE_VENDOR "Oracle and/or its affiliates")
SET(CPACK_PACKAGE_VERSION "1.1.0")
SET(CPACK_PACKAGE_VERSION_MAJOR "1")
SET(CPACK_PACKAGE_VERSION_MINOR "1")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_RESOURCE_FILE_LICENSE "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/COPYING")
SET(CPACK_RESOURCE_FILE_README "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/README")
SET(CPACK_RESOURCE_FILE_WELCOME "C:/Program Files/CMake 2.6/share/cmake-2.6/Templates/CPack.GenericWelcome.txt")
SET(CPACK_SET_DESTDIR "OFF")
SET(CPACK_SOURCE_CYGWIN "")
SET(CPACK_SOURCE_GENERATOR "TGZ")
SET(CPACK_SOURCE_IGNORE_FILES "/CMakeFiles/;/Testing/;/.bzr/;_CPack_Packages/;.cmake$;~;.swp;.log;.gz;.directory$;CMakeCache.txt;Makefile;install_manifest.txt;ANNOUNCEMENT_102_ALPHA;ANNOUNCEMENT_103_ALPHA;ANNOUNCEMENT_104_BETA;ANNOUNCEMENT_105_GA;ANNOUNCEMENT_110_GA;ANNOUNCEMENT_DRAFT;./cppconn/config.h$;./driver/nativeapi/binding_config.h$")
SET(CPACK_SOURCE_OUTPUT_CONFIG_FILE "D:/QT_Projects/WorkProjects_VS2008/mysqlconn/mysql-connector-c++-1.1.0/CPackSourceConfig.cmake")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "mysql-connector-c++-1.1.0")
SET(CPACK_SOURCE_TBZ2 "")
SET(CPACK_SOURCE_TGZ "")
SET(CPACK_SOURCE_TZ "")
SET(CPACK_SOURCE_ZIP "")
SET(CPACK_SYSTEM_NAME "win32")
SET(CPACK_TOPLEVEL_TAG "win32")
