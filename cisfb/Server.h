/*******************************************************************************
 * Copyright (c) 2023 Daniel Lindhuber
 * This program and the accompanying materials are made available under the
 * terms of the Eclipse Public License 2.0 which is available at
 * http://www.eclipse.org/legal/epl-2.0.
 *
 * SPDX-License-Identifier: EPL-2.0
 *
 * Contributors:
 * Daniel Lindhuber - initial API and implementation and/or initial documentation
 *******************************************************************************/

#pragma once

#include "base/BasePublisher.h"
#include "base/BaseSubscriber.h"

namespace forte::dds
{
    class Server final : public BasePublisher, public BaseSubscriber {
    public:
        Server(const std::string &topic, const std::string &profile, const callback_t &recv_callback)
            : BasePublisher(topic + "_outgoing", profile)
            , BaseSubscriber(topic + "_incoming", profile, recv_callback)
        {}
        ~Server() override;
        bool init(CIEC_ANY **input_pins, size_t input_size, CIEC_ANY **output_pins, size_t output_size);

    private:
        eprosima::fastdds::dds::DomainParticipant *mp_participant;
    };
}

