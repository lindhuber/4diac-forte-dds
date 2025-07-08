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

#ifndef FORTE_BASEPUBLISHER_H
#define FORTE_BASEPUBLISHER_H

#include <fastdds/dds/publisher/DataWriterListener.hpp>
#include <fastdds/dds/domain/DomainParticipant.hpp>
#include "forte_any.h"


class BasePublisher {
public:
    BasePublisher(const std::string &topic, const std::string &profile)
            : mp_participant(nullptr)
            , mp_publisher(nullptr)
            , m_profile(profile)
            , m_typename(topic) {}
    virtual ~BasePublisher();

    bool init(eprosima::fastdds::dds::DomainParticipant* participant, CIEC_ANY** pins, size_t size);
    bool publish(CIEC_ANY **mp_pins, size_t size) const;

    std::string m_typename;
    std::string m_profile;

private:
    static void setMemberData(eprosima::fastrtps::types::DynamicData_ptr data, CIEC_ANY *anyVal, int index);

    eprosima::fastrtps::types::DynamicData_ptr m_data;
    eprosima::fastdds::dds::DomainParticipant* mp_participant;
    eprosima::fastdds::dds::Publisher* mp_publisher;
    eprosima::fastdds::dds::Topic* mp_topic{};
    eprosima::fastdds::dds::DataWriter* mp_writer{};

    class PubListener : public eprosima::fastdds::dds::DataWriterListener
    {
    public:
        PubListener()
                : n_matched(0)
                , firstConnected(false)
        {
        }

        ~PubListener() override = default;

        void on_publication_matched(
                eprosima::fastdds::dds::DataWriter* writer,
                const eprosima::fastdds::dds::PublicationMatchedStatus& info) override;

        int n_matched;
        bool firstConnected;
    } m_listener;
};

#endif //FORTE_BASEPUBLISHER_H
