#
# Regular cron jobs for the pileofblocks package
#
0 4	* * *	root	[ -x /usr/bin/pileofblocks_maintenance ] && /usr/bin/pileofblocks_maintenance
