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


#ifndef DDSHANDLER_H_
#define DDSHANDLER_H_

#include <extevhan.h>
#include <fortelist.h>
#include <DDSComLayer.h>
#include <forte_sync.h>
#include <forte_string.h>
#include <forte_thread.h>
#include <forte_sem.h>
#include <string>
#include "posix/forte_sync.h"
#include "posix/forte_thread.h"
#include "posix/forte_sem.h"

class DDSHandler : public CExternalEventHandler, public CThread 
{
    DECLARE_HANDLER(DDSHandler)
public:
    int registerLayer(DDSComLayer* paLayer);
    void unregisterLayer(DDSComLayer* paLayer);

    virtual void enableHandler(void);
    /*!\brief Disable this event source
     */
    virtual void disableHandler(void);
    /*!\brief Sets the priority of the event source
     *
     * \param pa_nPriority new priority of the event source
     */
    virtual void setPriority(int pa_nPriority);
    /*!\brief Get the current priority of the event source
     *
     * \return current priority
     */
    virtual int getPriority(void) const;

    void startNewEventChain(DDSComLayer* layer);

protected:
    virtual void run();

protected:
    virtual void resumeSuspend();
    virtual void selfSuspend();
    virtual void addRegisteredLayers();
    virtual void removeDerigesteredLayers();

    typedef std::vector<DDSComLayer *> DDSFBContainer;

    static CSyncObject smDDSMutex;
    static forte::arch::CSemaphore mStateSemaphore;
    static bool mIsSemaphoreEmpty;

    int32_t partition = 0;
    std::vector<DDSComLayer *> m_toAdd;
    std::vector<DDSComLayer *> m_toRemove;
    DDSFBContainer mlayers;
};

#endif
