static void usage(void) {
    printf(PACKAGE " " VERSION "\n");
    printf("-p <num>      TCP port number to listen on (default: 11211)\n"
           "-U <num>      UDP port number to listen on (default: 11211, 0 is off)\n"
           "-s <file>     UNIX socket path to listen on (disables network support)\n"
           "-A            enable ascii \"shutdown\" command\n"
           "-a <mask>     access mask for UNIX socket, in octal (default: 0700)\n"
           "-l <addr>     interface to listen on (default: INADDR_ANY, all addresses)\n"
           "              <addr> may be specified as host:port. If you don't specify\n"
           "              a port number, the value you specified with -p or -U is\n"
           "              used. You may specify multiple addresses separated by comma\n"
           "              or by using -l multiple times\n"

           "-d            run as a daemon\n"
           "-r            maximize core file limit\n"
           "-u <username> assume identity of <username> (only when run as root)\n"
           "-m <num>      max memory to use for items in megabytes (default: 64 MB)\n"
           "-M            return error on memory exhausted (rather than removing items)\n"
           "-c <num>      max simultaneous connections (default: 1024)\n"
           "-k            lock down all paged memory.  Note that there is a\n"
           "              limit on how much memory you may lock.  Trying to\n"
           "              allocate more than that would fail, so be sure you\n"
           "              set the limit correctly for the user you started\n"
           "              the daemon with (not for -u <username> user;\n"
           "              under sh this is done with 'ulimit -S -l NUM_KB').\n"
           "-v            verbose (print errors/warnings while in event loop)\n"
           "-vv           very verbose (also print client commands/reponses)\n"
           "-vvv          extremely verbose (also print internal state transitions)\n"
           "-h            print this help and exit\n"
           "-i            print memcached and libevent license\n"
           "-P <file>     save PID in <file>, only used with -d option\n"
           "-f <factor>   chunk size growth factor (default: 1.25)\n"
           "-n <bytes>    minimum space allocated for key+value+flags (default: 48)\n");
    printf("-L            Try to use large memory pages (if available). Increasing\n"
           "              the memory page size could reduce the number of TLB misses\n"
           "              and improve the performance. In order to get large pages\n"
           "              from the OS, memcached will allocate the total item-cache\n"
           "              in one large chunk.\n");
    printf("-D <char>     Use <char> as the delimiter between key prefixes and IDs.\n"
           "              This is used for per-prefix stats reporting. The default is\n"
           "              \":\" (colon). If this option is specified, stats collection\n"
           "              is turned on automatically; if not, then it may be turned on\n"
           "              by sending the \"stats detail on\" command to the server.\n");
    printf("-t <num>      number of threads to use (default: 4)\n");
    printf("-R            Maximum number of requests per event, limits the number of\n"
           "              requests process for a given connection to prevent \n"
           "              starvation (default: 20)\n");
    printf("-C            Disable use of CAS\n");
    printf("-b            Set the backlog queue limit (default: 1024)\n");
    printf("-B            Binding protocol - one of ascii, binary, or auto (default)\n");
    printf("-I            Override the size of each slab page. Adjusts max item size\n"
           "              (default: 1mb, min: 1k, max: 128m)\n");
#ifdef ENABLE_SASL
    printf("-S            Turn on Sasl authentication\n");
#endif
    printf("-o            Comma separated list of extended or experimental options\n"
           "              - (EXPERIMENTAL) maxconns_fast: immediately close new\n"
           "                connections if over maxconns limit\n"
           "              - hashpower: An integer multiplier for how large the hash\n"
           "                table should be. Can be grown at runtime if not big enough.\n"
           "                Set this based on \"STAT hash_power_level\" before a \n"
           "                restart.\n"
           );
    return;
}