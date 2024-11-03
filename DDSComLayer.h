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


#ifndef DDSCOMLAYER_H_
#define DDSCOMLAYER_H_

#include "comlayer.h"
#include "../../core/datatypes/forte_string.h"
#include "com/dds/cisfb/Publisher.h"
#include "com/dds/cisfb/Subscriber.h"
#include "com/dds/cisfb/Client.h"
#include "com/dds/cisfb/Server.h"

using namespace forte::com_infra;

class DDSComLayer: public forte::com_infra::CComLayer {
public:

    DDSComLayer(CComLayer* paUpperLayer, CBaseCommFB * paFB);
    virtual ~DDSComLayer();

    EComResponse sendData(void* paData, unsigned int paSize);
    EComResponse recvData(const void *paData, unsigned int paSize);
    EComResponse processInterrupt();

private:
    EComResponse openConnection(char* paLayerParameter);
    void closeConnection();

    forte::dds::Subscriber* mp_sub;
    forte::dds::Publisher* mp_pub;
    forte::dds::Client* mp_client;
    forte::dds::Server* mp_server;

    EComResponse mInterruptResp;
};

#endif
