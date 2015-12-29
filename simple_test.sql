uninstall plugin mysqlbackup_plugin;
install plugin mysqlbackup_plugin soname 'mysqlbackup_plugin.so'; 
\W
backup server;
set mysqlbackup_plugin_backup_dir='$HOME/BACKUPDIR/mysqlbackup_plugin_test';
backup server;
set mysqlbackup_plugin_backup_tool='mysqlbackup';
backup server;
set mysqlbackup_plugin_backup_tool_basedir=concat(@@basedir, '/bin');
set mysqlbackup_plugin_backup_tool='mysqlpump';
backup server;
set mysqlbackup_plugin_backup_tool='xtrabackup';
backup server;
