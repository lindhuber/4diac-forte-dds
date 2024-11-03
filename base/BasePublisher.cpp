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

#include "BasePublisher.h"

#include <fastrtps/attributes/ParticipantAttributes.h>
#include <fastrtps/attributes/PublisherAttributes.h>
#include <fastdds/dds/publisher/Publisher.hpp>
#include <fastdds/dds/publisher/qos/PublisherQos.hpp>
#include <fastdds/dds/publisher/DataWriter.hpp>
#include <fastdds/dds/publisher/qos/DataWriterQos.hpp>
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

#include "util/Factory.h"

using namespace eprosima::fastdds::dds;
using namespace eprosima::fastrtps::types;

BasePublisher::~BasePublisher()
{
    if (mp_writer != nullptr) {
        mp_publisher->delete_datawriter(mp_writer);
    }
    if (mp_publisher != nullptr) {
        mp_participant->delete_publisher(mp_publisher);
    }
    if (mp_topic != nullptr) {
        mp_participant->delete_topic(mp_topic);
    }
}

bool BasePublisher::init(DomainParticipant* participant, CIEC_ANY** pins, size_t size) {
    DEVLOG_DEBUG("init pub \n");
    mp_participant = participant;

    mp_publisher = mp_participant->create_publisher(PUBLISHER_QOS_DEFAULT, nullptr);
    if (mp_publisher == nullptr) {
        return false;
    }

    m_data = forte::dds::Factory::create_dynamic_data(participant, pins, size, m_typename);
    if (m_data == nullptr) {
        return false;
    }
    
    mp_topic = mp_participant->create_topic(m_typename, m_typename, TOPIC_QOS_DEFAULT);
    if (mp_topic == nullptr) {
        return false;
    }

    mp_writer = mp_publisher->create_datawriter(mp_topic, DATAWRITER_QOS_DEFAULT, &m_listener);
    if (mp_writer == nullptr) {
        return false;
    }

    return true;
}

void BasePublisher::PubListener::on_publication_matched(
        eprosima::fastdds::dds::DataWriter*,
        const eprosima::fastdds::dds::PublicationMatchedStatus& info) {
    if (info.current_count_change == 1) {
        n_matched = info.total_count;
        firstConnected = true;
        std::cout << "Publisher matched" << std::endl;
    } else if (info.current_count_change == -1) {
        n_matched = info.total_count;
        std::cout << "Publisher unmatched" << std::endl;
    } else {
        std::cout << info.current_count_change
                  << " is not a valid value for PublicationMatchedStatus current count change" << std::endl;
    }
}

bool BasePublisher::publish(CIEC_ANY** pins, size_t size) {
    if (m_listener.firstConnected || m_listener.n_matched > 0) {
        for (int index = 0; index < size; ++index) {
            setMemberData(m_data, pins[index], index);
        }
        mp_writer->write(m_data.get());
        return true;
    }
    return false;
}

void BasePublisher::setMemberData(DynamicData_ptr data, CIEC_ANY *anyVal, int index)
{
    const CIEC_ANY &val = anyVal->unwrap();
    const uint8_t *data_ptr = val.getConstDataPtr();

    switch (val.getDataTypeID())
    {
        // boolean
        case CIEC_ANY::e_BOOL: {
            bool value = *reinterpret_cast<const bool *>(data_ptr);
            data->set_bool_value(value, index);
            break;
        }

            // signed
        case CIEC_ANY::e_SINT:
            // no 8 bit int in the dds spec -> fall through to 16 bit
        case CIEC_ANY::e_INT: {
            int16_t value = *reinterpret_cast<const int16_t *>(data_ptr);
            data->set_int16_value(value, index);
            break;
        }
        case CIEC_ANY::e_DINT: {
            int32_t value = *reinterpret_cast<const int32_t *>(data_ptr);
            data->set_int32_value(value, index);
            break;
        }
        case CIEC_ANY::e_LINT: {
            int64_t value = *reinterpret_cast<const int64_t *>(data_ptr);
            data->set_int64_value(value, index);
            break;
        }

            // unsigned
        case CIEC_ANY::e_USINT:
            // no 8 bit int in the dds spec -> fall through to 16 bit
        case CIEC_ANY::e_UINT: {
            uint16_t value = *reinterpret_cast<const uint16_t *>(data_ptr);
            data->set_uint16_value(value, index);
            break;
        }
        case CIEC_ANY::e_UDINT: {
            uint32_t value = *reinterpret_cast<const uint32_t *>(data_ptr);
            data->set_uint32_value(value, index);
            break;
        }
        case CIEC_ANY::e_ULINT: {
            uint64_t value = *reinterpret_cast<const uint64_t *>(data_ptr);
            data->set_uint64_value(value, index);
            break;
        }

            // floating point
        case CIEC_ANY::e_REAL: {
            float value = *reinterpret_cast<const float *>(data_ptr);
            data->set_float32_value(value, index);
            break;
        }
        case CIEC_ANY::e_LREAL: {
            double value = *reinterpret_cast<const double *>(data_ptr);
            data->set_float64_value(value, index);
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
            // CIEC_STRING value = *reinterpret_cast<const CIEC_STRING *>(data_ptr);
            // TODO
            data->set_int32_value(0, index);
            break;
        }
        case CIEC_ANY::e_WSTRING: {
            // TODO
            data->set_int32_value(0, index);
            break;
        }
        case CIEC_ANY::e_CHAR: {
            char value = *reinterpret_cast<const char *>(data_ptr);
            data->set_char8_value(value, index);
            break;
        }
        case CIEC_ANY::e_WCHAR: {
            char16_t value = *reinterpret_cast<const char16_t *>(data_ptr);
            data->set_char16_value(value, index);
            break;
        }

            // bit string
        case CIEC_ANY::e_BYTE: {
            uint8_t value = *reinterpret_cast<const uint8_t *>(data_ptr);
            data->set_byte_value(value, index);
            break;
        }
        case CIEC_ANY::e_WORD: {
            uint16_t value = *reinterpret_cast<const uint16_t *>(data_ptr);
            data->set_uint16_value(value, index);
            break;
        }
        case CIEC_ANY::e_DWORD: {
            uint32_t value = *reinterpret_cast<const uint32_t *>(data_ptr);
            data->set_uint32_value(value, index);
            break;
        }
        case CIEC_ANY::e_LWORD: {
            uint64_t value = *reinterpret_cast<const uint64_t *>(data_ptr);
            data->set_uint64_value(value, index);
            break;
        }

            // TODO: Enums, Arrays, Structs
        default:
            break;
    }
}
