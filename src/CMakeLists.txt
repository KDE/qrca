# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
#
# SPDX-License-Identifier: BSD-2-Clause

configure_file(config-qrca.h.in ${CMAKE_CURRENT_BINARY_DIR}/config-qrca.h)

set(qrca_SRCS
    main.cpp
    Qrca.cpp
    QrCodeScannerFilter.cpp
    QrCodeVideoFrame.cpp
    QrCodeDecoder.cpp
    QrCodeContent.cpp
    mecardparser.cpp
    notificationmanager.cpp
    clipboard.cpp
    resources.qrc
)

add_executable(qrca ${qrca_SRCS})

target_link_libraries(qrca Qt::Core Qt::Qml Qt::Quick Qt::Svg Qt::Multimedia Qt::Concurrent KF5::I18n KF5::Contacts KF5::CoreAddons KF5::Notifications)

if (TARGET ZXing::ZXing)
    target_link_libraries(qrca ZXing::ZXing)
elseif (TARGET ZXing::Core)
    target_link_libraries(qrca ZXing::Core)
endif()

target_compile_definitions(qrca PRIVATE -DQT_NO_CAST_FROM_ASCII)

if(ANDROID)
    target_link_libraries(qrca Qt::Svg KF5::Kirigami2)

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
        help-feedback
        internet-services
        itinerary
    )
else()
    target_link_libraries(qrca KF5::KIOGui KF5::Service)
endif()

install(TARGETS qrca ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
