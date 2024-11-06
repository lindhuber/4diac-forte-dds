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

#include "Factory.h"

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
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

#include <fastdds/dds/domain/DomainParticipantFactory.hpp>
#include <fastrtps/types/DynamicDataFactory.h>
#include <fastrtps/xmlparser/XMLProfileManager.h>

#include "forte_any.h"
#include "forte_bool.h"
#include "forte_byte.h"
#include "forte_dint.h"
#include "forte_dword.h"
#include "forte_int.h"
#include "forte_lint.h"
#include "forte_lreal.h"
#include "forte_lword.h"
#include "forte_real.h"
#include "forte_sint.h"
#include "forte_udint.h"
#include "forte_uint.h"
#include "forte_ulint.h"
#include "forte_usint.h"
#include "forte_word.h"
#include "forte_string.h"
#include "forte_wstring.h"
#include "forte_time.h"
#include "forte_time_of_day.h"
#include "forte_date_and_time.h"
#include "forte_date.h"
#include "forte_ltime.h"
#include "forte_ltime_of_day.h"
#include "forte_ldate_and_time.h"
#include "forte_ldate.h"
#include "forte_struct.h"
#include "forte_array.h"

#include "../../core/utils/parameterParser.h"

#include <filesystem>
#include <fstream>

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;


// participant->get_topic_qos_from_profile(profile, qos) == ReturnCode_t::RETCODE_OK

namespace forte::dds
{
    Factory& Factory::getInstance() 
    {
        static Factory instance;
        if (!instance.initialized_) 
        {
            instance.loadProfilesFromDirectory("../profiles");
            instance.initialized_ = true;
        }
        return instance;
    }   

    void Factory::loadProfilesFromDirectory(const std::string& directoryPath) 
    {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& entry : std::filesystem::directory_iterator(directoryPath)) 
        {
            if (entry.is_regular_file() && entry.path().extension() == ".xml") 
            {
                const std::string filePath = entry.path().string();
                DomainParticipantFactory::get_instance()->load_XML_profiles_file(filePath);
            }
        }
    }

    bool Factory::parseConfig(EntityConfig &config, const char *layer_params) 
    {
        CParameterParser parser(layer_params, ',', 2);
        if (parser.parseParameters() != 2) 
        {
            return false;
        }

        config.topic = parser[TopicName];
        config.profile = parser[ProfileName];

        return true;
    }

    eprosima::fastdds::dds::DomainParticipant *Factory::create_domain_participant() 
    {
        DomainParticipantQos participant_qos;
        DomainParticipantFactory::get_instance()->get_participant_qos_from_profile(
            "participant_xml_profile",
            participant_qos);

        DomainParticipantQos pqos;
        pqos.name("Participant_sub");
        StatusMask par_mask = StatusMask::subscription_matched() << StatusMask::data_available();

        return DomainParticipantFactory::get_instance()->create_participant(0, pqos);
    }

    eprosima::fastdds::dds::Publisher *Factory::create_publisher(DomainParticipant* participant, EntityConfig& config) 
    {
        PublisherQos qos;
        if (participant->get_publisher_qos_from_profile(config.profile, qos) != ReturnCode_t::RETCODE_OK) 
        {
            qos = PUBLISHER_QOS_DEFAULT;
        }
        return participant->create_publisher(qos);
    }

    eprosima::fastdds::dds::Subscriber *Factory::create_subscriber(DomainParticipant* participant, EntityConfig& config) 
    {
        SubscriberQos qos;
        if (participant->get_subscriber_qos_from_profile(config.profile, qos) != ReturnCode_t::RETCODE_OK) 
        {
            SubscriberQos
            qos = SUBSCRIBER_QOS_DEFAULT;
        }
        return participant->create_subscriber(qos);
    }

    eprosima::fastdds::dds::DataWriter *Factory::create_data_writer(Publisher* publisher, Topic* topic, EntityConfig& config) 
    {
        DataWriterQos qos;
        if (publisher->get_datawriter_qos_from_profile(config.profile, qos) != ReturnCode_t::RETCODE_OK) 
        {
            qos = DATAWRITER_QOS_DEFAULT;
        }
        return publisher->create_datawriter(topic, qos);
    }

    eprosima::fastdds::dds::DataReader *Factory::create_data_reader(Subscriber* subscriber, Topic* topic, EntityConfig& config) 
    {
        DataReaderQos qos;
        if (subscriber->get_datareader_qos_from_profile(config.profile, qos) != ReturnCode_t::RETCODE_OK) 
        {
            qos = DATAREADER_QOS_DEFAULT;
        }
        return subscriber->create_datareader(topic, qos);
    }

    eprosima::fastdds::dds::Topic *Factory::create_topic(DomainParticipant* participant, EntityConfig& config, std::string topic_name, std::string type_name) 
    {
        TopicQos qos;
        if (participant->get_topic_qos_from_profile(config.profile, qos) != ReturnCode_t::RETCODE_OK) 
        {
            qos = TOPIC_QOS_DEFAULT;
        }
        return participant->create_topic(topic_name, type_name, qos);
    }

    DynamicData_ptr Factory::create_dynamic_data(DomainParticipant* participant, CIEC_ANY** pins, size_t size, std::string type_name) 
    {
        DynamicTypeBuilder_ptr struct_type_builder(DynamicTypeBuilderFactory::get_instance()->create_struct_builder());

        for (size_t index = 0; index < size; index++) 
        {
            DynamicType_ptr type = Factory::createMemberType(pins[index]->unwrap().getDataTypeID());
            struct_type_builder->add_member(
                    index,
                    std::to_string(index),
                    type
            );
        }

        struct_type_builder->set_name(type_name);
        DynamicType_ptr dyn_type = struct_type_builder->build();

        TypeSupport m_type(new DynamicPubSubType(dyn_type));
        m_type.get()->auto_fill_type_information(true);
        m_type.get()->auto_fill_type_object(true);
        m_type.register_type(participant);

        return DynamicData_ptr(eprosima::fastrtps::types::DynamicDataFactory::get_instance()->create_data(dyn_type));
    }

    DynamicType_ptr Factory::createMemberType(CIEC_ANY::EDataTypeID typeID)
    {
        switch (typeID) 
        {
            // boolean
            case CIEC_ANY::e_BOOL:
                return DynamicTypeBuilderFactory::get_instance()->create_bool_type();

                // signed
            case CIEC_ANY::e_SINT:
                // no 8 bit int in the dds spec -> fall through to 16 bit
            case CIEC_ANY::e_INT:
                return DynamicTypeBuilderFactory::get_instance()->create_int16_type();
            case CIEC_ANY::e_DINT:
                return DynamicTypeBuilderFactory::get_instance()->create_int32_type();
            case CIEC_ANY::e_LINT:
                return DynamicTypeBuilderFactory::get_instance()->create_int64_type();

                // unsigned
            case CIEC_ANY::e_USINT:
                // no 8 bit int in the dds spec -> fall through to 16 bit
            case CIEC_ANY::e_UINT:
                return DynamicTypeBuilderFactory::get_instance()->create_uint16_type();
            case CIEC_ANY::e_UDINT:
                return DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
            case CIEC_ANY::e_ULINT:
                return DynamicTypeBuilderFactory::get_instance()->create_uint64_type();

                // floating point
            case CIEC_ANY::e_REAL:
                return DynamicTypeBuilderFactory::get_instance()->create_float32_type();
            case CIEC_ANY::e_LREAL:
                return DynamicTypeBuilderFactory::get_instance()->create_float64_type();

                // date and time
            case CIEC_ANY::e_TIME:
            case CIEC_ANY::e_LTIME:
            case CIEC_ANY::e_DATE:
            case CIEC_ANY::e_LDATE:
            case CIEC_ANY::e_TIME_OF_DAY:
            case CIEC_ANY::e_LTIME_OF_DAY:
            case CIEC_ANY::e_DATE_AND_TIME:
            case CIEC_ANY::e_LDATE_AND_TIME:
                break;

                // char and string
            case CIEC_ANY::e_STRING:
                return DynamicTypeBuilderFactory::get_instance()->create_string_type();
            case CIEC_ANY::e_WSTRING:
                return DynamicTypeBuilderFactory::get_instance()->create_wstring_type();
            case CIEC_ANY::e_CHAR:
                return DynamicTypeBuilderFactory::get_instance()->create_char8_type();
            case CIEC_ANY::e_WCHAR:
                return DynamicTypeBuilderFactory::get_instance()->create_char16_type();

                // bit string
            case CIEC_ANY::e_BYTE:
                return DynamicTypeBuilderFactory::get_instance()->create_byte_type();
            case CIEC_ANY::e_WORD:
                return DynamicTypeBuilderFactory::get_instance()->create_uint16_type();
            case CIEC_ANY::e_DWORD:
                return DynamicTypeBuilderFactory::get_instance()->create_uint32_type();
            case CIEC_ANY::e_LWORD:
                return DynamicTypeBuilderFactory::get_instance()->create_uint64_type();

                // TODO: Enums, Arrays, Structs
            default:
                // default for now
                return DynamicTypeBuilderFactory::get_instance()->create_uint64_type();
        }
    }
}

