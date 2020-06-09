project(qrca)

cmake_minimum_required(VERSION 2.8.12)
set(QT_MIN_VERSION "5.5.0")

option(BUILD_TESTING "Build tests" ON)

################# Disallow in-source build #################

if("${CMAKE_SOURCE_DIR}" STREQUAL "${CMAKE_BINARY_DIR}")
   message(FATAL_ERROR "This application requires an out of source build. Please create a separate build directory.")
endif()

include(FeatureSummary)

################# set KDE specific information #################

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMPoQmTools)
include(KDECompilerSettings NO_POLICY_SCOPE)
kde_enable_exceptions()

################# Find dependencies #################

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Test Gui Svg QuickControls2 Multimedia DBus)
find_package(KF5 REQUIRED COMPONENTS Kirigami2 Purpose I18n Contacts CoreAddons)
find_package(ZXing REQUIRED)

################# Enable C++11 features for clang and gcc #################

set(CMAKE_CXX_STANDARD 11)

################# build and install #################
add_subdirectory(src)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(PROGRAMS org.kde.qrca.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.qrca.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.qrca.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
