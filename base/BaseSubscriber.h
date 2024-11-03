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

#ifndef FORTE_BASESUBSCRIBER_H
#define FORTE_BASESUBSCRIBER_H

#include <fastdds/dds/domain/DomainParticipant.hpp>
#include <fastdds/dds/domain/DomainParticipantListener.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastrtps/subscriber/SampleInfo.h>
#include <fastrtps/rtps/common/Types.h>

#include <fastrtps/types/TypeIdentifier.h>
#include <fastrtps/types/TypeObject.h>

#include <fastrtps/attributes/SubscriberAttributes.h>

#include <atomic>
#include <condition_variable>
#include <map>
#include "forte_any.h"

#include <functional>

#include "comtypes.h"

#include "util/EntityConfig.h"

class BaseSubscriber {
public:
    BaseSubscriber(EntityConfig config, std::string topic_name)
            : m_config(config)
            , m_typename(topic_name)
            , mp_participant(nullptr)
            , mp_subscriber(nullptr)
            , mp_topic(nullptr)
            , mp_reader(nullptr)
            , m_listener(this, config.recvCallback) {}
    virtual ~BaseSubscriber();

    bool init(eprosima::fastdds::dds::DomainParticipant* mp_participant, CIEC_ANY** pins, size_t size);
    bool apply(CIEC_ANY **pins, unsigned int size);

    EntityConfig m_config;

    std::string m_typename;

    eprosima::fastdds::dds::DomainParticipant* mp_participant;
    eprosima::fastdds::dds::Subscriber* mp_subscriber;
    eprosima::fastdds::dds::Topic* mp_topic;
    eprosima::fastdds::dds::DataReader* mp_reader;

    eprosima::fastrtps::types::DynamicType_ptr mp_type;
    eprosima::fastrtps::types::DynamicData_ptr mp_data;
    eprosima::fastrtps::SubscriberAttributes m_sub_attr;
    eprosima::fastdds::dds::DataReaderQos m_reader_qos;

public:
    class ParticipantListener : public eprosima::fastdds::dds::DomainParticipantListener {
    public:
        ParticipantListener(BaseSubscriber* sub, std::function<forte::com_infra::EComResponse(const void*, unsigned int)> recvCallback)
                : mp_struct_subscriber(sub)
                , recvCallback(recvCallback) {}
        ~ParticipantListener() override = default;

        void on_data_available(eprosima::fastdds::dds::DataReader* reader) override;

        BaseSubscriber* mp_struct_subscriber;
        std::function<forte::com_infra::EComResponse(const void*,  unsigned int)> recvCallback;

    } m_listener;

};

#endif //FORTE_BASESUBSCRIBER_H
