/*
 * *****************************************************************************
 *   Copyright 2014 Spectra Logic Corporation. All Rights Reserved.
 *   Licensed under the Apache License, Version 2.0 (the "License"). You may not
 *   use this file except in compliance with the License. A copy of the License
 *   is located at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *   or in the "license" file accompanying this file.
 *   This file is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
 *   CONDITIONS OF ANY KIND, either express or implied. See the License for the
 *   specific language governing permissions and limitations under the License.
 * *****************************************************************************
 */

#ifndef LOGGER_H
#define LOGGER_H

#include "views/console.h"

#define LOG_DEBUG(msg)   LOG(Console::DEBUG,   msg)
#define LOG_INFO(msg)    LOG(Console::INFO,    msg)
#define LOG_WARNING(msg) LOG(Console::WARNING, msg)
#define LOG_ERROR(msg)   LOG(Console::ERROR,   msg)
#define LOG(level, msg)  Console::Instance()->Log(level, msg)

#endif
