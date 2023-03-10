# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
#
# SPDX-License-Identifier: BSD-2-Clause

if (TARGET KF${QT_MAJOR_VERSION}::NetworkManagerQt)
    set(HAVE_NETWORKMANAGER TRUE)
endif()
configure_file(config-qrca.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-qrca.h)

set(qrca_SRCS
    main.cpp
    Qrca.cpp
    QrCodeContent.cpp
    mecardparser.cpp
    notificationmanager.cpp
    clipboard.cpp
    resources.qrc
)

add_executable(qrca ${qrca_SRCS})

target_link_libraries(qrca Qt::Core Qt::Qml Qt::Quick Qt::Svg Qt::Multimedia KF${QT_MAJOR_VERSION}::I18n KF${QT_MAJOR_VERSION}::Contacts KF${QT_MAJOR_VERSION}::CoreAddons KF${QT_MAJOR_VERSION}::Notifications KF${QT_MAJOR_VERSION}::PrisonScanner)

if (TARGET KF${QT_MAJOR_VERSION}::NetworkManagerQt)
    target_link_libraries(qrca KF${QT_MAJOR_VERSION}::NetworkManagerQt)
endif()

target_compile_definitions(qrca PRIVATE -DQT_NO_CAST_FROM_ASCII)

if(ANDROID)
    target_link_libraries(qrca Qt::Svg KF${QT_MAJOR_VERSION}::Kirigami2)

    kirigami_package_breeze_icons(ICONS
        arrow-right
        camera-photo-symbolic
        camera-video-symbolic
        document-encrypt
        document-new-symbolic
        document-save
        document-share
        edit-clear
        edit-copy-symbolic
        flashlight-off
        flashlight-on
        help-feedback
        internet-services
        itinerary
        network-wireless
    )
else()
    target_link_libraries(qrca KF${QT_MAJOR_VERSION}::KIOGui KF${QT_MAJOR_VERSION}::Service)
endif()

install(TARGETS qrca ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
