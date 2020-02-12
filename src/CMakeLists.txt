set(qrca_SRCS
    main.cpp
    Qrca.cpp
    QrCodeScannerFilter.cpp
    QrCodeVideoFrame.cpp
    QrCodeDecoder.cpp
    resources.qrc
    )

add_executable(qrca ${qrca_SRCS})
target_link_libraries(qrca Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg Qt5::Multimedia ZXing::Core KF5::I18n KF5::Contacts KF5::CoreAddons)

if(ANDROID)
    target_link_libraries(qrca Qt5::Svg KF5::Kirigami2)

    kirigami_package_breeze_icons(ICONS
        document-share
        document-save
        edit-clear
        arrow-right
        camera-photo-symbolic
        document-new-symbolic
        help-feedback
    )
endif()

install(TARGETS qrca ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
