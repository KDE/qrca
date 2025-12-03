# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
#
# SPDX-License-Identifier: BSD-2-Clause
cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "25")
set (RELEASE_SERVICE_VERSION_MINOR "12")
set (RELEASE_SERVICE_VERSION_MICRO "0")
set (RELEASE_SERVICE_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")
project(qrca VERSION ${RELEASE_SERVICE_VERSION})

set(QT_MIN_VERSION "6.6.0")
set(KF_MIN_VERSION "6.12.0")
set(KDE_COMPILERSETTINGS_LEVEL "5.82")

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMPoQmTools)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(KDEClangFormat)
include(ECMAddAndroidApk)
include(ECMQmlModule)
include(ECMDeprecationSettings)
kde_enable_exceptions()

ecm_setup_version(${RELEASE_SERVICE_VERSION} VARIABLE_PREFIX QRCA
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/src/qrca_version.h")

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Test Gui Svg QuickControls2 Multimedia)
find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Kirigami I18n Contacts CoreAddons Notifications Prison Config)
find_package(KF6KirigamiAddons REQUIRED)

if(NOT ANDROID)
    find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS Service KIO Crash DBusAddons)
    find_package(KF6 ${KF_MIN_VERSION} OPTIONAL_COMPONENTS NetworkManagerQt)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

ecm_set_disabled_deprecation_versions(
    QT 6.8.0
    KF 6.12.0
)

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

if (NOT ANDROID)
    install(PROGRAMS org.kde.qrca.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    if (TARGET KF6::NetworkManagerQt)
        install(PROGRAMS org.kde.qrca.wifi.desktop DESTINATION ${KDE_INSTALL_APPDIR})
    endif()
    install(FILES org.kde.qrca.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
    install(FILES org.kde.qrca.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)
endif()

ki18n_install(po)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
