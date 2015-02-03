/*  Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.

    This program is free software; you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by the
    Free Software Foundation; version 2 of the License.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
    02110-1301  USA */
#ifndef MYSQL_SERVER
#define MYSQL_SERVER
#endif

#include <ctype.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <mysql/plugin.h>
#include <mysql/plugin_query_rewrite.h>
#include <typelib.h>

using namespace std;
static MYSQL_PLUGIN plugin_info_ptr;

static char* backup_dir_value;
ulong backup_tool_name;
static char* backup_tool_basedir_value;
static char* backup_tool_options_value;

static const char* supported_tools[] = {
    "mysqlbackup",
    "mysqldump",
    NullS
};

static TYPELIB supported_tools_typelib = {
    array_elements(supported_tools) - 1,
    "supported_tools_typelib",
    supported_tools,
    NULL
};

enum supported_tools_t {
    MYSQLBACKUP,
    MYSQLDUMP
};

static int mysqlbackup_plugin_init(MYSQL_PLUGIN plugin_ref)
{
  plugin_info_ptr= plugin_ref;
  return 0;
}

static int perform_backup(Mysql_rewrite_pre_parse_param *param)
{
  if (0 == strcasecmp("backup server", param->query))
  {

    if (!backup_dir_value)
    {
      const char* newq= "SELECT 'You must set global variable mysqlbackup_plugin_backup_dir before running this command!'";
      param->rewritten_query= new char[strlen(newq) + 1];
      param->rewritten_query_length=strlen(newq);
      strncpy(param->rewritten_query, newq, param->rewritten_query_length + 1);
    }
    else
    {
      ostringstream oss;
      switch(backup_tool_name) {
	  case MYSQLBACKUP:
	      oss << "SELECT '" << supported_tools[backup_tool_name] << " not supported yet.'";
	      param->rewritten_query_length= strlen(oss.str().c_str());
	      param->rewritten_query= new char[param->rewritten_query_length + 1];
	      strncpy(param->rewritten_query, oss.str().c_str(), param->rewritten_query_length + 1);
	      break;
	  case MYSQLDUMP:
	      cerr << "Processing mysqldump" << endl;
	      if (backup_tool_basedir_value && 0 < strlen(backup_tool_basedir_value))
		  oss << "SELECT run_external(concat('" << backup_tool_basedir_value << "/" << supported_tools[backup_tool_name] << " " << backup_tool_options_value << " --socket=', @@socket, ' --all-databases > " << backup_dir_value << "/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))";
	      else
		  oss << "SELECT run_external(concat('" << supported_tools[backup_tool_name] << " " << backup_tool_options_value << " --socket=', @@socket, ' --all-databases > " << backup_dir_value << "/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))";
	      cerr << oss.str() << endl;
	      param->rewritten_query_length= strlen(oss.str().c_str());
	      param->rewritten_query= new char[param->rewritten_query_length + 1];
	      strncpy(param->rewritten_query, oss.str().c_str(), param->rewritten_query_length + 1);
	      break;
      }
    }

    param->flags|= FLAG_REWRITE_PLUGIN_QUERY_REWRITTEN;
  }
  return 0;
}

static int free_resources(Mysql_rewrite_pre_parse_param *param)
{
  delete [] param->rewritten_query;
  param->rewritten_query= NULL;
  param->rewritten_query_length= 0;
  return 0;
}

static st_mysql_rewrite_pre_parse mysqlbackup_plugin_descriptor= {
  MYSQL_REWRITE_PRE_PARSE_INTERFACE_VERSION,    /* interface version          */
  perform_backup,                               /* performs backup            */
  free_resources,                               /* frees allocated resources  */
};

/* System variables */

static MYSQL_SYSVAR_STR(backup_dir, backup_dir_value, PLUGIN_VAR_MEMALLOC, "Default directory where to store backup", NULL, NULL, NULL);
static MYSQL_SYSVAR_ENUM(backup_tool, backup_tool_name, PLUGIN_VAR_RQCMDARG, "Backup tool. Possible values: mysqldump|mysqlbackup", NULL, NULL, MYSQLDUMP, &supported_tools_typelib);
static MYSQL_SYSVAR_STR(backup_tool_basedir, backup_tool_basedir_value, PLUGIN_VAR_MEMALLOC, "Base dir for backup tool. Default: \"\"", NULL, NULL, "");
static MYSQL_SYSVAR_STR(backup_tool_options, backup_tool_options_value, PLUGIN_VAR_MEMALLOC, "Options for backup tool", NULL, NULL, "");

static struct st_mysql_sys_var *mysqlbackup_plugin_vars[] = {
    MYSQL_SYSVAR(backup_dir),
    MYSQL_SYSVAR(backup_tool),
    MYSQL_SYSVAR(backup_tool_basedir),
    MYSQL_SYSVAR(backup_tool_options),
    NULL
};

mysql_declare_plugin(mysqlbackup_plugin)
{
  MYSQL_REWRITE_PRE_PARSE_PLUGIN,
  &mysqlbackup_plugin_descriptor,
  "mysqlbackup_plugin",
  "Sveta Smirnova",
  "Plugin which provides SQL interface for MySQL Enterprise Backup and mysqldump",
  PLUGIN_LICENSE_GPL,
  mysqlbackup_plugin_init,
  NULL,
  0x0001,                                       /* version 0.0.1      */
  NULL,                                         /* status variables   */
  mysqlbackup_plugin_vars,                      /* system variables   */
  NULL,                                         /* config options     */
  0,                                            /* flqgs              */
}
mysql_declare_plugin_end;
