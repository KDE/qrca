include(ECMAddTests)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

include_directories(../src)

ecm_add_test(qrcatest.cpp ../src/Qrca.cpp
             TEST_NAME qrcatest
             LINK_LIBRARIES Qt5::Test Qt5::Core Qt5::Gui ZXing::Core KF5::I18n KF5::Contacts
)
