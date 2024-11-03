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

#ifndef FORTE_SERVER_H
#define FORTE_SERVER_H

#include "base/BasePublisher.h"
#include "base/BaseSubscriber.h"
#include "util/EntityConfig.h"

namespace forte::dds
{
    class Server : public BasePublisher, public BaseSubscriber {
    public:
        Server(EntityConfig config)
            : BasePublisher(config, config.topic + "_outgoing")
            , BaseSubscriber(config, config.topic + "_incoming")
        {}
        ~Server();
        bool init(CIEC_ANY **input_pins, size_t input_size, CIEC_ANY **output_pins, size_t output_size);

    private:
        eprosima::fastdds::dds::DomainParticipant *mp_participant;
    };
}

#endif //FORTE_SERVER_H
