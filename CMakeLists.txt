# Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.
# Copyright (c) 2015, Sveta Smirnova. All rights reserved.
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 2 of the License.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

IF(DEFINED MYSQL_NO_DASH_VERSION)
    IF("5.7.4" VERSION_LESS ${MYSQL_NO_DASH_VERSION})
    MYSQL_ADD_PLUGIN(mysqlbackup_plugin mysqlbackup_plugin.cc
      MODULE_ONLY MODULE_OUTPUT_NAME "mysqlbackup_plugin")
  ENDIF()
ENDIF()
