#############################################################################
# Copyright (c) 2023 Daniel Lindhuber
# This program and the accompanying materials are made available under the
# terms of the Eclipse Public License 2.0 which is available at
# http://www.eclipse.org/legal/epl-2.0.
#
# SPDX-License-Identifier: EPL-2.0
#
# Contributors:
# Daniel Lindhuber - initial API and implementation and/or initial documentation
#############################################################################

forte_add_network_layer(DDS OFF "dds" DDSComLayer DDSComLayer "Enable DDS Com Layer")
forte_add_include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(FORTE_COM_DDS)
    cmake_policy(SET CMP0057 NEW)

    forte_add_handler(DDSHandler DDSHandler)
    forte_add_sourcefile_hcpp(DDSComLayer DDSHandler)

    if(NOT fastdds_FOUND)
      find_package(fastdds REQUIRED)
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
        # MSVC (RTTI is enabled by default)
        forte_add_definition("/EHsc") # Equivalent to exceptions flag
    else()
        # GCC/Clang
        forte_add_definition("-fexceptions")
        forte_add_definition("-frtti")
    endif()

    if(TARGET fastdds)
        forte_add_include_directories(${fastdds_INCLUDE_DIRS})
    endif()

    forte_add_link_library(fastdds)

    forte_add_sourcefile_h(util/EntityConfig.h)
    forte_add_sourcefile_hcpp(util/Factory)
    forte_add_sourcefile_hcpp(base/BasePublisher base/BaseSubscriber)
    forte_add_sourcefile_hcpp(cisfb/Publisher cisfb/Subscriber cisfb/Client cisfb/Server)
endif(FORTE_COM_DDS)

