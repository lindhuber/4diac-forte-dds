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

#include "Publisher.h"
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include "util/Factory.h"

namespace forte::dds
{
    Publisher::~Publisher() {
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
    }

    bool Publisher::init(CIEC_ANY **pins, size_t size) {
        eprosima::fastdds::dds::DomainParticipantQos pqos;
        pqos.name("Participant_pub");
        mp_participant = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(0, pqos);
        if (mp_participant == nullptr) {
            return false;
        }

        return BasePublisher::init(mp_participant, pins, size);
    }

}