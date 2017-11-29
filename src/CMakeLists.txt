#   Copyright 2016 Comcast Cable Communications Management, LLC
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
set(PROJ_RESOLV_LIB ucresolv)

file(GLOB HEADERS ucresolv.h ucresolv_log.h)
set(SOURCES res_init.c res_query.c res_mkquery.c res_send.c
  inet_pton.c inet6_scopeid_pton.c inet_addr.c inet_mkadr.c
  if_index.c mempcpy.c ns_name.c
  ../include/glibc-stdlib/strtoull_l.c
  ../include/glibc-stdlib/strtol_l.c
  ../include/glibc-stdlib/grouping.c
  ../include/locale/xlocale.c
  ../include/locale/C-address.c
  ../include/locale/C-collate.c
  ../include/locale/C-ctype.c
  ../include/locale/C-identification.c
  ../include/locale/C-measurement.c
  ../include/locale/C-messages.c
  ../include/locale/C-monetary.c
  ../include/locale/C-name.c
  ../include/locale/C-numeric.c
  ../include/locale/C-paper.c
  ../include/locale/C-telephone.c
  ../include/locale/C-time.c
  ../include/locale/C_name.c
)

add_library(${PROJ_RESOLV_LIB} STATIC ${HEADERS} ${SOURCES})
add_library(${PROJ_RESOLV_LIB}.shared SHARED ${HEADERS} ${SOURCES})
target_link_libraries(${PROJ_RESOLV_LIB} pthread)
set_target_properties(${PROJ_RESOLV_LIB}.shared PROPERTIES OUTPUT_NAME ${PROJ_RESOLV_LIB})

install (TARGETS ${PROJ_RESOLV_LIB} DESTINATION lib${LIB_SUFFIX})
install (TARGETS ${PROJ_RESOLV_LIB}.shared DESTINATION lib${LIB_SUFFIX})
install (FILES ucresolv.h ucresolv_log.h DESTINATION include/${PROJ_RESOLV_LIB})
