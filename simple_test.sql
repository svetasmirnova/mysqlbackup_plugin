uninstall plugin mysqlbackup_plugin;
install plugin mysqlbackup_plugin soname 'mysqlbackup_plugin.so'; 
set global mysqlbackup_plugin_backup_dir='$HOME/src/BACKUPDIR/mysqlbackup_plugin_test';
\W
backup server;

