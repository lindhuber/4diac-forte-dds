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

#ifndef FORTE_ENTITYCONFIG_H
#define FORTE_ENTITYCONFIG_H

#include <string>
#include "forte_any.h"
#include <functional>
#include "comtypes.h"

struct EntityConfig
{
    std::string topic;
    std::string profile;
    std::function<forte::com_infra::EComResponse(const void*, unsigned int)> recvCallback;
};

#endif //FORTE_ENTITYCONFIG_H
