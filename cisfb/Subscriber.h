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

#ifndef FORTE_SUBSCRIBER_H
#define FORTE_SUBSCRIBER_H

#include "base/BaseSubscriber.h"

namespace forte::dds
{
    class Subscriber : public BaseSubscriber {
    public:
        Subscriber(EntityConfig config)
            : mp_participant(nullptr)
            , BaseSubscriber(config, config.topic)
        {}
        ~Subscriber();
        bool init(CIEC_ANY **pins, size_t size);

    private:
        eprosima::fastdds::dds::DomainParticipant *mp_participant;
    };
}

#endif //FORTE_SUBSCRIBER_H
