set(qrskanner_SRCS
    main.cpp
    QrCodeScanner.cpp
    QrSkanner.cpp
    )

qt5_add_resources(RESOURCES resources.qrc)
add_executable(qrskanner ${qrskanner_SRCS} ${RESOURCES})
target_link_libraries(qrskanner Qt5::Core Qt5::Qml Qt5::Quick Qt5::Svg Qt5::Multimedia ZXing::Core KF5::I18n)
install(TARGETS qrskanner ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})
