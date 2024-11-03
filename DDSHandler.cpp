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


#include "DDSHandler.h"
#include "../../core/cominfra/commfb.h"
#include <criticalregion.h>
#include <string>
#include <algorithm>

DEFINE_HANDLER(DDSHandler);

CSyncObject DDSHandler::smDDSMutex;
forte::arch::CSemaphore DDSHandler::mStateSemaphore = forte::arch::CSemaphore();
bool DDSHandler::mIsSemaphoreEmpty = true;

DDSHandler::DDSHandler(CDeviceExecution& paDeviceExecution)
        : CExternalEventHandler(paDeviceExecution)
{
    enableHandler();
}

DDSHandler::~DDSHandler()
{
    disableHandler();
}

int DDSHandler::registerLayer(DDSComLayer* paLayer) {
    CCriticalRegion section(smDDSMutex);
    m_toAdd.push_back(paLayer);
    enableHandler();
    return 0;
}

void DDSHandler::unregisterLayer(DDSComLayer *paLayer) {
    CCriticalRegion section(smDDSMutex);
    m_toRemove.push_back(paLayer);
}

void DDSHandler::enableHandler() {
    if(!isAlive()){
        start();
    }
}

void DDSHandler::disableHandler() {
    if (isAlive()){
        setAlive(false);
        //resumeSuspend();
        end();
    }
}

void DDSHandler::setPriority(int pa_prio) {
    // nothing to do here
}

int DDSHandler::getPriority() const {
  return 0;
}

void DDSHandler::run() {
    DDSFBContainer::iterator it = mlayers.begin();
    DDSComLayer* layer;
    while (isAlive()) {
        if (it != mlayers.end()) {
            layer = *it;
            // layer->recvData(nullptr, 0);
            sleepThread(250);
            it++;
        }

        if (it == mlayers.end()) {
            if (!m_toAdd.empty()) {
                addRegisteredLayers();
            }
            if (!m_toRemove.empty()) {
                removeDerigesteredLayers();
            }
            if (mlayers.empty()) {
                sleepThread(50);
                if(isAlive()){
                    //disableHandler();
                }
            }
            it = mlayers.begin();
        }
    }
}

void DDSHandler::addRegisteredLayers() {
    CCriticalRegion section(smDDSMutex);
    DDSFBContainer::iterator it = m_toAdd.begin();
    while(it != m_toAdd.end()){
        mlayers.push_back(*it);
        ++it;
    }
    m_toAdd.clear();
}

void DDSHandler::removeDerigesteredLayers() {
    CCriticalRegion section(smDDSMutex);
    DDSFBContainer::iterator it_remove = m_toRemove.begin();
    while(it_remove != m_toRemove.end()){
        DDSFBContainer::iterator it = std::find(mlayers.begin(),mlayers.end(),*it_remove);
        if(it != mlayers.end()){
            mlayers.erase(it);
        }
        ++it_remove;
    }
    m_toRemove.clear();
}

void DDSHandler::resumeSuspend() {
    if(mIsSemaphoreEmpty){
        mStateSemaphore.inc();
        mIsSemaphoreEmpty = false;
    }
}

void DDSHandler::selfSuspend() {
    mStateSemaphore.waitIndefinitely();
}

void DDSHandler::startNewEventChain(DDSComLayer* layer) {
    CExternalEventHandler::startNewEventChain(layer->getCommFB());
}
