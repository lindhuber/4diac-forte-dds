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

#ifndef FORTE_DDSCONFIGPARSER_H
#define FORTE_DDSCONFIGPARSER_H

#include "EntityConfig.h"
#include <fastdds/dds/domain/DomainParticipant.hpp>

#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>

#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/DataReader.hpp>
#include <fastdds/dds/subscriber/qos/SubscriberQos.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;

namespace forte::dds
{
    class Factory 
    {
    public:
        static DomainParticipant *create_domain_participant();
        static Publisher *create_publisher(DomainParticipant* participant, EntityConfig& config);
        static Subscriber *create_subscriber(DomainParticipant* participant, EntityConfig& config);
        static DataWriter *create_data_writer(Publisher* publisher, Topic* topic, EntityConfig& config);
        static DataReader *create_data_reader(Subscriber* subscriber, Topic* topic, EntityConfig& config);
        static Topic *create_topic(DomainParticipant* participant, EntityConfig& config, std::string topic_name, std::string type_name);

        static DynamicData_ptr create_dynamic_data(DomainParticipant *participant, CIEC_ANY **pins, size_t size, std::string type_name);

        static bool parseConfig(EntityConfig &config, const char *layer_params);

    private:
        Factory() = default;
        ~Factory() = default;
        Factory(const Factory&) = delete;
        Factory& operator=(const Factory&) = delete;
        static Factory& getInstance();

        void loadProfilesFromDirectory(const std::string& directoryPath);

        std::unordered_map<std::string, std::string> profiles_;
        std::mutex mutex_;
        bool initialized_ = false;

        static DynamicType_ptr createMemberType(CIEC_ANY::EDataTypeID typeID);

        static const size_t PARAMS_NUM = 2;
        enum Parameters 
        {
            TopicName,
            ProfileName
        };
    };
}

#endif //FORTE_DDSCONFIGPARSER_H
