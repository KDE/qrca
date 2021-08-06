# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>
#
# SPDX-License-Identifier: BSD-2-Clause

include(ECMAddTests)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

include_directories(../src)

ecm_add_test(qrcatest.cpp ../src/Qrca.cpp ../src/QrCodeContent.cpp ../src/mecardparser.cpp
             TEST_NAME qrcatest
             LINK_LIBRARIES Qt::Test Qt::Core Qt::Gui KF5::I18n KF5::Contacts KF5::CoreAddons
)
if (TARGET ZXing::ZXing)
     target_link_libraries(qrcatest ZXing::ZXing)
elseif (TARGET ZXing::Core)
     target_link_libraries(qrcatest ZXing::Core)
endif()
if (NOT ANDROID)
    target_link_libraries(qrcatest KF5::KIOGui KF5::Service)
endif()

ecm_add_test(mecardparsertest.cpp ../src/mecardparser.cpp TEST_NAME mecardparsertest LINK_LIBRARIES Qt5::Core Qt5::Test)
