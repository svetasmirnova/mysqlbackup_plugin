/*  Copyright (c) 2014, Oracle and/or its affiliates. All rights reserved.
    Copyright (c) 2015, Sveta Smirnova. All rights reserved.

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
#include <mysql/plugin_audit.h>
#include <mysql/service_mysql_alloc.h>
#include <my_thread.h> // my_thread_handle needed by mysql_memory.h
#include <mysql/psi/mysql_memory.h>
#include <typelib.h>

using namespace std;
static MYSQL_PLUGIN plugin_info_ptr;

/* instrument the memory allocation */
#ifdef HAVE_PSI_INTERFACE
static PSI_memory_key key_memory_mysqlbackup;

static PSI_memory_info all_rewrite_memory[]=
{
    { &key_memory_mysqlbackup, "mysqlbackup", 0 } 
};

static int mysqlbackup_plugin_init(MYSQL_PLUGIN plugin_ref)
{
  plugin_info_ptr= plugin_ref;
  const char* category= "sql";
  int count;

  count= array_elements(all_rewrite_memory);
  mysql_memory_register(category, all_rewrite_memory, count);
  return 0; /* success */
}
#else
#define plugin_init NULL
#define key_memory_mysqlbackup PSI_NOT_INSTRUMENTED
#endif /* HAVE_PSI_INTERFACE */

//static char* backup_dir_value;
//ulong backup_tool_name;
//static char* backup_tool_basedir_value;
//static char* backup_tool_options_value;

static const char* supported_tools[] = {
    "mysqlbackup",
    "mysqldump",
    "mysqlpump",
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
    MYSQLDUMP,
    MYSQLPUMP
};

/* System variables */

static MYSQL_THDVAR_STR(backup_dir, /* backup_dir_value, */ PLUGIN_VAR_MEMALLOC, "Default directory where to store backup", NULL, NULL, NULL);
static MYSQL_THDVAR_ENUM(backup_tool, /* backup_tool_name, */ PLUGIN_VAR_RQCMDARG, "Backup tool. Possible values: mysqldump|mysqlbackup|mysqlpump", NULL, NULL, MYSQLDUMP, &supported_tools_typelib);
static MYSQL_THDVAR_STR(backup_tool_basedir, /* backup_tool_basedir_value, */ PLUGIN_VAR_MEMALLOC, "Base dir for backup tool. Default: \"\"", NULL, NULL, "");
static MYSQL_THDVAR_STR(backup_tool_options, /* backup_tool_options_value, */ PLUGIN_VAR_MEMALLOC, "Options for backup tool", NULL, NULL, "");


static struct st_mysql_sys_var *mysqlbackup_plugin_sys_vars[] = {
    MYSQL_SYSVAR(backup_dir),
    MYSQL_SYSVAR(backup_tool),
    MYSQL_SYSVAR(backup_tool_basedir),
    MYSQL_SYSVAR(backup_tool_options),
    NULL
};

static int perform_backup(MYSQL_THD thd, mysql_event_class_t event_class,
                          const void *event)
{
  const struct mysql_event_parse *event_parse=
    static_cast<const struct mysql_event_parse *>(event);

  if (event_parse->event_subclass != MYSQL_AUDIT_PARSE_PREPARSE)
      return 0;
    
  const char* backup_dir_value= (const char *) THDVAR(thd, backup_dir);
  ulong backup_tool_name= (ulong) THDVAR(thd, backup_tool);
  const char* backup_tool_basedir_value= (const char*) THDVAR(thd, backup_tool_basedir);
  const char* backup_tool_options_value= (const char*) THDVAR(thd, backup_tool_options);

  if (0 == strcasecmp("backup server",  event_parse->query.str))
  {

    if (!backup_dir_value)
    {
      const char* newq= "SELECT 'You must set variable mysqlbackup_plugin_backup_dir before running this command!'";
      char *rewritten_query= static_cast<char *>(my_malloc(key_memory_mysqlbackup, strlen(newq) + 1, MYF(0)));
      strncpy(rewritten_query, newq, strlen(newq) + 1);
      event_parse->rewritten_query->str= rewritten_query;
      event_parse->rewritten_query->length=strlen(newq);
    }
    else
    {
      ostringstream oss;
      char *rewritten_query;
      switch(backup_tool_name) {
        case MYSQLBACKUP:
          oss << "SELECT '" << supported_tools[backup_tool_name] << " not supported yet.'";
          rewritten_query= static_cast<char *>(my_malloc(key_memory_mysqlbackup, strlen(oss.str().c_str()) + 1, MYF(0))); 
          strncpy(rewritten_query, oss.str().c_str(), strlen(oss.str().c_str()));
          event_parse->rewritten_query->str= rewritten_query;
          event_parse->rewritten_query->length= strlen(oss.str().c_str());
          break;
        case MYSQLDUMP:
          cerr << "Processing mysqldump" << endl;
          if (backup_tool_basedir_value && 0 < strlen(backup_tool_basedir_value))
            oss << "SELECT run_external(concat('" << backup_tool_basedir_value << "/" << supported_tools[backup_tool_name] << " " << backup_tool_options_value << " --socket=', @@socket, ' --all-databases > " << backup_dir_value << "/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))";
          else
            oss << "SELECT run_external(concat('" << supported_tools[backup_tool_name] << " " << backup_tool_options_value << " --socket=', @@socket, ' --all-databases > " << backup_dir_value << "/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))";
          cerr << oss.str() << endl;
          rewritten_query= static_cast<char *>(my_malloc(key_memory_mysqlbackup, strlen(oss.str().c_str()) + 1, MYF(0)));
          strncpy(rewritten_query, oss.str().c_str(), strlen(oss.str().c_str()));
          event_parse->rewritten_query->str= rewritten_query;
          event_parse->rewritten_query->length= strlen(oss.str().c_str());
          break;
        case MYSQLPUMP:
        default:
          oss << "SELECT '" << supported_tools[backup_tool_name] << " not supported yet.'";
          rewritten_query= static_cast<char *>(my_malloc(key_memory_mysqlbackup, strlen(oss.str().c_str()) + 1, MYF(0))); 
          strncpy(rewritten_query, oss.str().c_str(), strlen(oss.str().c_str()));
          event_parse->rewritten_query->str= rewritten_query;
          event_parse->rewritten_query->length= strlen(oss.str().c_str());
          break;
      }
    }

    *((int *)event_parse->flags)|= (int)MYSQL_AUDIT_PARSE_REWRITE_PLUGIN_QUERY_REWRITTEN;
  }
  return 0;
}

static st_mysql_audit mysqlbackup_plugin_descriptor= {
  MYSQL_AUDIT_INTERFACE_VERSION,                 /* interface version          */
  NULL,
  perform_backup,                               /* performs backup            */
  { 0, 0, (unsigned long) MYSQL_AUDIT_PARSE_ALL,}
};

mysql_declare_plugin(mysqlbackup_plugin)
{
  MYSQL_AUDIT_PLUGIN,
  &mysqlbackup_plugin_descriptor,
  "mysqlbackup_plugin",
  "Sveta Smirnova",
  "Plugin which provides SQL interface for MySQL Enterprise Backup and mysqldump",
  PLUGIN_LICENSE_GPL,
  mysqlbackup_plugin_init,
  NULL,                                         /* mysqlbackup_plugin_deinit - TODO */
  0x0002,                                       /* version 0.0.2      */
  NULL,                                         /* status variables   */
  mysqlbackup_plugin_sys_vars,                  /* system variables   */
  NULL,                                         /* config options     */
  0,                                            /* flqgs              */
}
mysql_declare_plugin_end;

/* vim: set tabstop=2 shiftwidth=2 softtabstop=2: */
/* 
* :tabSize=2:indentSize=2:noTabs=true:
* :folding=custom:collapseFolds=1: 
*/
