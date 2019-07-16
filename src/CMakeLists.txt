set(qrca_SRCS
    main.cpp
    Qrca.cpp
    QrCodeScannerFilter.cpp
    QrCodeVideoFrame.cpp
    QrCodeDecoder.cpp
    )

qt5_add_resources(RESOURCES resources.qrc)
add_executable(qrca ${qrca_SRCS} ${RESOURCES})
target_link_libraries(qrca Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg Qt5::Multimedia ZXing::Core KF5::I18n KF5::Contacts)
install(TARGETS qrca ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
