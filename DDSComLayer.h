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

#include "comlayer.h"
#include "../../core/datatypes/forte_string.h"
#include "./cisfb/Publisher.h"
#include "./cisfb/Subscriber.h"
#include "./cisfb/Client.h"
#include "./cisfb/Server.h"

using namespace forte::com_infra;

class DDSComLayer final : public forte::com_infra::CComLayer
{
public:

    DDSComLayer(CComLayer* paUpperLayer, CBaseCommFB * paFB);

    ~DDSComLayer() override;

    EComResponse sendData(void* paData, unsigned int paSize) override;
    EComResponse recvData(const void *paData, unsigned int paSize) override;
    EComResponse processInterrupt() override;

private:
    EComResponse openConnection(char* paLayerParameter) override;
    void closeConnection() override;

    forte::dds::Subscriber* mp_sub;
    forte::dds::Publisher* mp_pub;
    forte::dds::Client* mp_client;
    forte::dds::Server* mp_server;

    EComResponse mInterruptResp;
};

