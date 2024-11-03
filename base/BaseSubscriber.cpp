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

#include "BaseSubscriber.h"

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/SubscriberAttributes.h>
#include <fastdds/dds/subscriber/Subscriber.hpp>
#include <fastdds/dds/subscriber/SampleInfo.hpp>
#include <fastdds/dds/subscriber/qos/DataReaderQos.hpp>
#include <fastdds/dds/domain/DomainParticipantFactory.hpp>

#include <fastrtps/types/DynamicDataHelper.hpp>
#include <fastrtps/types/DynamicDataFactory.h>
#include <mutex>

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

#include "util/Factory.h"

using namespace eprosima::fastdds::dds;
using eprosima::fastrtps::types::ReturnCode_t;

BaseSubscriber::~BaseSubscriber()
{
    if (mp_subscriber != nullptr) {
        if (mp_reader != nullptr) {
            mp_subscriber->delete_datareader(mp_reader);
        }
        if (mp_topic != nullptr) {
            mp_participant->delete_topic(mp_topic);
        }
        mp_participant->delete_subscriber(mp_subscriber);
    }
}

bool BaseSubscriber::init(DomainParticipant* participant, CIEC_ANY** pins, size_t size) {
    DEVLOG_DEBUG("init sub\n");
    mp_participant = participant;

    // mp_participant->set_listener(&m_listener, StatusMask::data_available());

    m_reader_qos = DATAREADER_QOS_DEFAULT;
    m_reader_qos.reliability().kind = RELIABLE_RELIABILITY_QOS;

    mp_subscriber = mp_participant->create_subscriber(SUBSCRIBER_QOS_DEFAULT, nullptr);
    if (mp_subscriber == nullptr) {
        return false;
    }

    mp_data = forte::dds::Factory::create_dynamic_data(participant, pins, size, m_typename);
    if (mp_data == nullptr) {
        return false;
    }

    mp_topic = mp_participant->create_topic(m_typename, m_typename, TOPIC_QOS_DEFAULT);
    if (mp_topic == nullptr) {
        return false;
    }

    mp_reader = mp_subscriber->create_datareader(
            mp_topic,
            m_reader_qos,
            &m_listener,
            StatusMask::data_available());
    if (mp_reader == nullptr) {
        return false;
    }

    return true;
}

void BaseSubscriber::ParticipantListener::on_data_available(DataReader* reader) {
    DEVLOG_DEBUG("data available \n");
    SampleInfo info;
    if (reader->take_next_sample(mp_struct_subscriber->mp_data.get(), &info) == ReturnCode_t::RETCODE_OK) {
        if (info.instance_state == ALIVE_INSTANCE_STATE) {
            eprosima::fastrtps::types::DynamicDataHelper::print(mp_struct_subscriber->mp_data);
            recvCallback(nullptr, 0);
        }
    }
}

bool BaseSubscriber::apply(CIEC_ANY **pins, unsigned int size) {
    for (int index = 0; index < size; ++index) {
        CIEC_ANY *anyVal = pins[index];
        switch (anyVal->unwrap().getDataTypeID())
        {
            // boolean
            case CIEC_ANY::e_BOOL: {
                bool value = mp_data->get_bool_value(index);
                static_cast<CIEC_BOOL&>(anyVal->unwrap()) = CIEC_BOOL(value);
                break;
            }

            // signed
            case CIEC_ANY::e_SINT:
                // no 8 bit int in the dds spec -> fall through to 16 bit
            case CIEC_ANY::e_INT: {
                int16_t value = mp_data->get_int16_value(index);
                static_cast<CIEC_INT&>(anyVal->unwrap()) = CIEC_INT(value);
                break;
            }
            case CIEC_ANY::e_DINT: {
                int32_t value = mp_data->get_int32_value(index);
                static_cast<CIEC_DINT&>(anyVal->unwrap()) = CIEC_DINT(value);
                break;
            }
            case CIEC_ANY::e_LINT: {
                int64_t value = mp_data->get_int64_value(index);
                static_cast<CIEC_LINT&>(anyVal->unwrap()) = CIEC_LINT(value);
                break;
            }

            // unsigned
            case CIEC_ANY::e_USINT:
                // no 8 bit int in the dds spec -> fall through to 16 bit
            case CIEC_ANY::e_UINT: {
                uint16_t value = mp_data->get_uint16_value(index);
                static_cast<CIEC_UINT&>(anyVal->unwrap()) = CIEC_UINT(value);
                break;
            }
            case CIEC_ANY::e_UDINT: {
                uint32_t value = mp_data->get_uint32_value(index);
                static_cast<CIEC_UDINT&>(anyVal->unwrap()) = CIEC_UDINT(value);
                break;
            }
            case CIEC_ANY::e_ULINT: {
                uint64_t value = mp_data->get_uint64_value(index);
                static_cast<CIEC_ULINT&>(anyVal->unwrap()) = CIEC_ULINT(value);
                break;
            }

            // floating point
            case CIEC_ANY::e_REAL: {
                float value = mp_data->get_float32_value(index);
                static_cast<CIEC_REAL&>(anyVal->unwrap()) = CIEC_REAL(value);
                break;
            }
            case CIEC_ANY::e_LREAL: {
                double value = mp_data->get_float64_value(index);
                static_cast<CIEC_LREAL&>(anyVal->unwrap()) = CIEC_LREAL(value);
                break;
            }

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
            case CIEC_ANY::e_STRING: {
                // TODO
                break;
            }
            case CIEC_ANY::e_WSTRING: {
                // TODO
                break;
            }
            case CIEC_ANY::e_CHAR: {
                char value = mp_data->get_char8_value(index);
                static_cast<CIEC_CHAR&>(anyVal->unwrap()) = CIEC_CHAR(value);
                break;
            }
            case CIEC_ANY::e_WCHAR: {
                char16_t value = mp_data->get_char16_value(index);
                static_cast<CIEC_WCHAR&>(anyVal->unwrap()) = CIEC_WCHAR(value);
                break;
            }

                // bit string
            case CIEC_ANY::e_BYTE: {
                uint8_t value = mp_data->get_byte_value(index);
                static_cast<CIEC_BYTE&>(anyVal->unwrap()) = CIEC_BYTE(value);
                break;
            }
            case CIEC_ANY::e_WORD: {
                uint16_t value = mp_data->get_uint16_value(index);
                static_cast<CIEC_WORD&>(anyVal->unwrap()) = CIEC_WORD(value);
                break;
            }
            case CIEC_ANY::e_DWORD: {
                uint32_t value = mp_data->get_uint32_value(index);
                static_cast<CIEC_DWORD&>(anyVal->unwrap()) = CIEC_DWORD(value);
                break;
            }
            case CIEC_ANY::e_LWORD: {
                uint64_t value = mp_data->get_uint64_value(index);
                static_cast<CIEC_LWORD&>(anyVal->unwrap()) = CIEC_LWORD(value);
                break;
            }
            // TODO: Enums, Arrays, Structs
            default:
                break;
        }
    }
    return true;
}
