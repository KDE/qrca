# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
#
# SPDX-License-Identifier: BSD-2-Clause

if (TARGET KF6::NetworkManagerQt)
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

target_link_libraries(qrca Qt::Core Qt::Qml Qt::Quick Qt::Svg Qt::Multimedia KF6::I18n KF6::Contacts KF6::CoreAddons KF6::Notifications KF6::PrisonScanner)

if (TARGET KF6::NetworkManagerQt)
    target_link_libraries(qrca KF6::NetworkManagerQt)
endif()

target_compile_definitions(qrca PRIVATE -DQT_NO_CAST_FROM_ASCII)

if(ANDROID)
    target_link_libraries(qrca Qt::Svg KF6::Kirigami2)

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
    target_link_libraries(qrca Qt::Widgets KF6::KIOGui KF6::Service)
endif()

install(TARGETS qrca ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
