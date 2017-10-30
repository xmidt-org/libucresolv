/**
 * Copyright 2017 Comcast Cable Communications Management, LLC
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *     http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef __UCRESOLV_LOG_H__
#define __UCRESOLV_LOG_H__

#include <stdarg.h>

#define LEVEL_ERROR    0
#define LEVEL_INFO     1
#define LEVEL_DEBUG    2

#define LOGGING_MODULE "UCRESOLV"

/**
* @brief handle log message based on log level
* 
* @param module string identifying library/module
* @param level of log is info,debug,error
* @param msg message
*/
typedef void (*logger_func_t) (const char *module, int level, const char *msg, ...);

extern logger_func_t logger_func;

void register_ucresolv_logger (logger_func_t logger_func_p);

#define ucresolv_error(...) \
  if (NULL != logger_func) logger_func(LOGGING_MODULE, LEVEL_ERROR, __VA_ARGS__)
#define ucresolv_info(...) \
  if (NULL != logger_func) logger_func(LOGGING_MODULE, LEVEL_INFO, __VA_ARGS__)
#define ucresolv_debug(...) \
  if (NULL != logger_func) logger_func(LOGGING_MODULE, LEVEL_DEBUG, __VA_ARGS__)

#endif
