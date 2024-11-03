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

#include "Subscriber.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include "util/Factory.h"

namespace forte::dds
{
    Subscriber::~Subscriber() {
        eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->delete_participant(mp_participant);
    }

    bool Subscriber::init(CIEC_ANY **pins, size_t size) {
        // mp_participant = fordiac::dds::entities::create_domain_participant();
        mp_participant = eprosima::fastdds::dds::DomainParticipantFactory::get_instance()->create_participant(0, PARTICIPANT_QOS_DEFAULT);
        if (mp_participant == nullptr) {
            return false;
        }

        return BaseSubscriber::init(mp_participant, pins, size);
    }
}