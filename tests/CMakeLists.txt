# SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
# SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
# SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>
#
# SPDX-License-Identifier: BSD-2-Clause

include(ECMAddTests)

find_package(Qt6 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Test)

include_directories(../src ${CMAKE_CURRENT_BINARY_DIR}/../src)

ecm_add_test(qrcatest.cpp ../src/Qrca.cpp ../src/QrCodeContent.cpp ../src/mecardparser.cpp
             TEST_NAME qrcatest
             LINK_LIBRARIES Qt::Test Qt::Core Qt::Gui KF6::I18n KF6::Contacts KF6::CoreAddons KF6::PrisonScanner
)
if (NOT ANDROID)
    target_link_libraries(qrcatest KF6::KIOGui KF6::Service)
endif()
if (TARGET KF6::NetworkManagerQt)
    target_link_libraries(qrcatest KF6::NetworkManagerQt)
endif()

ecm_add_test(mecardparsertest.cpp ../src/mecardparser.cpp TEST_NAME mecardparsertest LINK_LIBRARIES Qt::Core Qt::Test)
