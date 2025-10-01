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
    notificationmanager.cpp
    clipboard.cpp
)

add_executable(qrca ${qrca_SRCS})

ecm_add_qml_module(qrca URI "org.kde.qrca" VERSION 1.0)

ecm_target_qml_sources(qrca SOURCES
    qml/Main.qml
    qml/QrCodeScannerPage.qml
    qml/QrCodeEncoderPage.qml
    qml/AboutPage.qml
)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../icons/*.png")
ecm_add_app_icon(qrca ICONS ${ICONS_SRCS})

target_link_libraries(qrca PRIVATE Qt::Core Qt::Qml Qt::Quick Qt::QuickControls2 Qt::Svg Qt::Multimedia KF6::I18n KF6::Contacts KF6::CoreAddons KF6::Notifications KF6::Prison KF6::PrisonScanner KF6::I18nQml)

if (TARGET KF6::NetworkManagerQt)
    target_link_libraries(qrca PRIVATE KF6::NetworkManagerQt)
endif()

target_compile_definitions(qrca PRIVATE -DQT_NO_CAST_FROM_ASCII)

if(ANDROID)
    target_link_libraries(qrca PRIVATE Qt::Svg KF6::Kirigami)

    kirigami_package_breeze_icons(ICONS
        ${KIRIGAMI_ADDONS_ICONS}
        arrow-right
        camera-photo-symbolic
        camera-video-symbolic
        document-encrypt
        document-new-symbolic
        document-open
        document-save
        document-share
        edit-clear
        edit-copy-symbolic
        flashlight-off
        flashlight-on
        help-feedback
        internet-services
        itinerary
        license
        network-wireless
    )
    install(FILES ${CMAKE_SOURCE_DIR}/org.kde.qrca.svg  DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kirigami/breeze-internal/icons/)
    ecm_add_android_apk(qrca ANDROID_DIR ${CMAKE_SOURCE_DIR}/android)
else()
    target_link_libraries(qrca PRIVATE Qt::Widgets KF6::KIOGui KF6::Service KF6::Crash KF6::DBusAddons)
endif()

install(TARGETS qrca ${KF_INSTALL_TARGETS_DEFAULT_ARGS})
