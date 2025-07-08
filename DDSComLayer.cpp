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


#include "DDSComLayer.h"
#include "DDSHandler.h"
#include "commfb.h"
#include <functional>
#include <dds/domain/DomainParticipantFactory.hpp>

#include "parameterParser.h"
#include "util/Factory.h"

using namespace forte::com_infra;

DDSComLayer::DDSComLayer(CComLayer* paUpperLayer, CBaseCommFB * pFB)
        : CComLayer(paUpperLayer, pFB)
        , mInterruptResp(e_Nothing)
        , mp_sub(nullptr)
        , mp_pub(nullptr)
        , mp_client(nullptr)
        , mp_server(nullptr)
{
}

DDSComLayer::~DDSComLayer() = default;

EComResponse DDSComLayer::openConnection(char* paLayerParameter) 
{
    EComResponse eRetVal = e_InitInvalidId;

    if (getBottomLayer() != nullptr || getTopLayer() != nullptr) 
    {
        return e_InitInvalidId;
    }

    DomainParticipantFactory::get_instance()->load_XML_profiles_file("profiles/test.xml");

    CParameterParser parser(paLayerParameter, ',', 2);
    if (parser.parseParameters() != 2)
    {
        return e_InitInvalidId;
    }

    const std::string topic = parser[0];
    const std::string profile = parser[1];
    auto recv_callback = std::bind(&DDSComLayer::recvData, this, std::placeholders::_1, std::placeholders::_2);

    switch(getCommFB()->getComServiceType())
    {
        case e_Publisher:
        {
            mp_pub = new forte::dds::Publisher(topic, profile);
            if (mp_pub->init(getCommFB()->getSDs(), getCommFB()->getNumSD()))
            {
                getExtEvHandler<DDSHandler>().registerLayer(this);
                eRetVal = e_InitOk;
            }
            break;
        }
        case e_Subscriber:
        {
            mp_sub = new forte::dds::Subscriber(topic, profile, recv_callback);
            if (mp_sub->init(getCommFB()->getRDs(), getCommFB()->getNumRD()))
            {
                getExtEvHandler<DDSHandler>().registerLayer(this);
                eRetVal = e_InitOk;
            }
            break;
        }
        case e_Server:
        {
            mp_server = new forte::dds::Server(topic, profile, recv_callback);
            if (mp_server->init(getCommFB()->getSDs(), getCommFB()->getNumSD(), getCommFB()->getRDs(), getCommFB()->getNumRD()))
            {
                getExtEvHandler<DDSHandler>().registerLayer(this);
                eRetVal = e_InitOk;
            }
            break;
        }
        case e_Client:
        {
            mp_client = new forte::dds::Client(topic, profile, recv_callback);
            if (mp_client->init(getCommFB()->getSDs(), getCommFB()->getNumSD(), getCommFB()->getRDs(), getCommFB()->getNumRD())) 
            {
                getExtEvHandler<DDSHandler>().registerLayer(this);
                eRetVal = e_InitOk;
            }
            break;
        }
        default:
            eRetVal = e_InitTerminated;
            break;
    }
    return eRetVal;
}

void DDSComLayer::closeConnection() 
{
    // nothing to do for now
}

EComResponse DDSComLayer::sendData(void* paData, unsigned int paSize) 
{
    switch(getCommFB()->getComServiceType()) 
    {
        case e_Publisher: 
        {
            return mp_pub->publish(getCommFB()->getSDs(), getCommFB()->getNumSD()) ? e_ProcessDataOk : e_ProcessDataSendFailed;
        }
        case e_Server: 
        {
            return mp_server->publish(getCommFB()->getSDs(), getCommFB()->getNumSD()) ? e_ProcessDataOk : e_ProcessDataSendFailed;
        }
        case e_Client: 
        {
            return mp_client->publish(getCommFB()->getSDs(), getCommFB()->getNumSD()) ? e_ProcessDataOk : e_ProcessDataSendFailed;
        }
        default:
            return e_ProcessDataNoSocket;
    }
}

EComResponse DDSComLayer::recvData(const void* paData,  unsigned int paSize) 
{
    getCommFB()->interruptCommFB(this);
    getExtEvHandler<DDSHandler>().startNewEventChain(this);
    return e_ProcessDataOk;
}

EComResponse DDSComLayer::processInterrupt() 
{
    switch(getCommFB()->getComServiceType()) 
    {
        case e_Subscriber: 
        {
            mp_sub->apply(getCommFB()->getRDs(), getCommFB()->getNumRD());
            return mInterruptResp;
        }
        case e_Server: 
        {
            mp_server->apply(getCommFB()->getRDs(), getCommFB()->getNumRD());
            return mInterruptResp;
        }
        case e_Client: 
        {
            mp_client->apply(getCommFB()->getRDs(), getCommFB()->getNumRD());
            return mInterruptResp;
        }
        default:
            return mInterruptResp;
    }
}
