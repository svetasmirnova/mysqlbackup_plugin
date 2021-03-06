# mysqlbackup_plugin
Introduction
============

This plugin provides SQL interface for MySQL backup tools. Currently it supports mysqldump, MySQL Enterprise Backup, Percona XtraBackup and mysqlpump. There are plans to support other tools. It provides single statement: BACKUP SERVER which makes full backup of the server. There are plans to support different types of backups and create own locks to prevent BACKUP SERVER to be called from multiple threads.

Plugin explores [Query Rewrite Pre-Parse Plugin] (http://mysqlserverteam.com/write-yourself-a-query-rewrite-plugin-part-1/), thus only available for MySQL 5.7 or newer.

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
    +----------------------------------------------------------------------------------+
    | You must set variable mysqlbackup_plugin_backup_dir before running this command! |
    +----------------------------------------------------------------------------------+
    | You must set variable mysqlbackup_plugin_backup_dir before running this command! |
    +----------------------------------------------------------------------------------+
    1 row in set, 1 warning (0.00 sec)
    
    mysql> set mysqlbackup_plugin_backup_dir='$HOME/src/BACKUPDIR/plugin_test';
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
    
    mysql> set mysqlbackup_plugin_backup_tool_basedir='$HOME/build/mysql-trunk/bin';
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
