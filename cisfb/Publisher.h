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

namespace forte::dds
{
    class Publisher final : public BasePublisher {
    public:
        Publisher(const std::string &topic, const std::string &profile)
            : mp_participant(nullptr)
            , BasePublisher(topic, profile)
        {}
        ~Publisher() override;
        bool init(CIEC_ANY **pins, size_t size);

    private:
        eprosima::fastdds::dds::DomainParticipant *mp_participant;
    };
}

