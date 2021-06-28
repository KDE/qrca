# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
#
# SPDX-License-Identifier: BSD-2-Clause

project(qrca)

cmake_minimum_required(VERSION 3.16)
set(QT_MIN_VERSION "5.15.0")
set(KF5_MIN_VERSION "5.78")

find_package(ECM ${KF5_MIN_VERSION} REQUIRED NO_MODULE)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH})

include(FeatureSummary)
include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(KDEInstallDirs)
include(KDECMakeSettings)
include(ECMPoQmTools)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(KDEClangFormat)
kde_enable_exceptions()

find_package(Qt5 ${QT_MIN_VERSION} REQUIRED NO_MODULE COMPONENTS Core Quick Test Gui Svg QuickControls2 Multimedia Concurrent)
find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Kirigami2 I18n Contacts CoreAddons Config)
find_package(ZXing REQUIRED)

if(NOT ANDROID)
    find_package(KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS Purpose)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(PROGRAMS org.kde.qrca.desktop DESTINATION ${KDE_INSTALL_APPDIR})
install(FILES org.kde.qrca.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
install(FILES org.kde.qrca.svg DESTINATION ${KDE_INSTALL_FULL_ICONDIR}/hicolor/scalable/apps/)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)

file(GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES *.cpp *.h)
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
