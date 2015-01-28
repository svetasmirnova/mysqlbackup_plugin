# mysqlbackup_plugin
Introduction
============

This plugin provides SQL interface for MySQL backup tools. Currently it only supports BACKUP SERVER statement and mysqldump. There are plans to support MySQL Enterprise Backup and other tools.

Plugin explores [Query Rewrite Pre-Parse Plugin] (http://mysqlserverteam.com/write-yourself-a-query-rewrite-plugin-part-1/) To use it you need [mysql-5.7.5-labs-preview] (mysql-5.7.5-labs-preview) package from [MySQL Labs] (http://labs.mysql.com/)

Plugin also explores [run_external UDF] (https://github.com/svetasmirnova/run_external) You need to compile it and install before using plugin.

Compiling
=========

Place plugin directory into plugin subdirectory of MySQL source directory, then cd to MySQL source directory and compile server. Plugin will be compiled automatically.

Installation
============

Copy mysqlbackup_plugin.so into plugin directory of your MySQL server, then login and type:

    INSTALL PLUGIN mysqlbackup_plugin SONAME 'mysqlbackup_plugin.so';

Uninstallation
==============

Connect to MySQL server and run:

    UNINSTALL PLUGIN mysqlbackup_plugin;

Usage examples
==============

    mysql> show variables like 'mysqlbackup%';
    +----------------------------------------+-----------+
    | Variable_name                          | Value     |
    +----------------------------------------+-----------+
    | mysqlbackup_plugin_backup_dir          |           |
    | mysqlbackup_plugin_backup_tool         | mysqldump |
    | mysqlbackup_plugin_backup_tool_basedir |           |
    | mysqlbackup_plugin_backup_tool_options |           |
    +----------------------------------------+-----------+
    4 rows in set (0.00 sec)
    
    mysql> backup server;
    +-----------------------------------------------------------------------------------------+
    | You must set global variable mysqlbackup_plugin_backup_dir before running this command! |
    +-----------------------------------------------------------------------------------------+
    | You must set global variable mysqlbackup_plugin_backup_dir before running this command! |
    +-----------------------------------------------------------------------------------------+
    1 row in set, 1 warning (0.00 sec)
    
    mysql> set global mysqlbackup_plugin_backup_dir='$HOME/src/BACKUPDIR/plugin_test';
    Query OK, 0 rows affected (0.00 sec)
    
    mysql> backup server;
    +-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | run_external(concat('mysqldump  --socket=', @@socket, ' --all-databases > $HOME/src/BACKUPDIR/plugin_test/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql')) |
    +-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |                                                                                                                                                                     1 |
    +-----------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set, 1 warning (0.60 sec)
    
    mysql> show warnings;
    +-------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Level | Code | Message                                                                                                                                                                                                                                          |
    +-------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Note  | 1105 | Query 'backup server' rewritten to 'SELECT run_external(concat('mysqldump  --socket=', @@socket, ' --all-databases > $HOME/src/BACKUPDIR/plugin_test/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))' by plugin: mysqlbackup_plugin. |
    +-------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)
    
    mysql> set global mysqlbackup_plugin_backup_tool_basedir='$HOME/build/mysql-trunk/bin';
    Query OK, 0 rows affected (0.00 sec)
    
    mysql> backup server;
    +---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | run_external(concat('$HOME/build/mysql-trunk/bin/mysqldump  --socket=', @@socket, ' --all-databases > $HOME/src/BACKUPDIR/plugin_test/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql')) |
    +---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |                                                                                                                                                                                                 1 |
    +---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set, 1 warning (0.57 sec)
    
    mysql> show warnings;
    +-------+------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Level | Code | Message                                                                                                                                                                                                                                                                      |
    +-------+------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Note  | 1105 | Query 'backup server' rewritten to 'SELECT run_external(concat('$HOME/build/mysql-trunk/bin/mysqldump  --socket=', @@socket, ' --all-databases > $HOME/src/BACKUPDIR/plugin_test/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))' by plugin: mysqlbackup_plugin. |
    +-------+------+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)
    
    uery OK, 0 rows affected (0.00 sec)
    
    mysql> backup server;
    +-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | run_external(concat('$HOME/build/mysql-trunk/bin/mysqldump --single-transaction --socket=', @@socket, ' --all-databases > $HOME/src/BACKUPDIR/plugin_test/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql')) |
    +-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    |                                                                                                                                                                                                                     1 |
    +-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set, 1 warning (0.57 sec)
    
    mysql> show warnings;
    +-------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Level | Code | Message                                                                                                                                                                                                                                                                                          |
    +-------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    | Note  | 1105 | Query 'backup server' rewritten to 'SELECT run_external(concat('$HOME/build/mysql-trunk/bin/mysqldump --single-transaction --socket=', @@socket, ' --all-databases > $HOME/src/BACKUPDIR/plugin_test/backup_', date_format(now(), '%Y-%m-%e_%H:%i:%s'), '.sql'))' by plugin: mysqlbackup_plugin. |
    +-------+------+--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
    1 row in set (0.00 sec)
