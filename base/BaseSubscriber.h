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

typedef std::function<forte::com_infra::EComResponse(const void*,  unsigned int)> callback_t;

class BaseSubscriber {
public:
    BaseSubscriber(const std::string &topic, const std::string &profile, callback_t recv_callback)
            : m_profile(profile)
            , m_typename(topic)
            , mp_participant(nullptr)
            , mp_subscriber(nullptr)
            , mp_topic(nullptr)
            , mp_reader(nullptr)
            , m_listener(this, recv_callback) {}
    virtual ~BaseSubscriber();

    bool init(eprosima::fastdds::dds::DomainParticipant* mp_participant, CIEC_ANY** pins, size_t size);
    bool apply(CIEC_ANY **pins, unsigned int size) const;

    std::string m_typename;
    std::string m_profile;

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
        ParticipantListener(BaseSubscriber* sub, const callback_t &recv_callback)
                : mp_struct_subscriber(sub)
                , recv_callback(recv_callback) {}
        ~ParticipantListener() override = default;

        void on_data_available(eprosima::fastdds::dds::DataReader* reader) override;

        BaseSubscriber* mp_struct_subscriber;
        callback_t recv_callback;

    } m_listener;

};

