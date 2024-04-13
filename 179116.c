void usage() {
	printf("This is nbd-server version " VERSION "\n");
	printf("Usage: port file_to_export [size][kKmM] [-l authorize_file] [-r] [-m] [-c] [-a timeout_sec]\n"
	       "\t-r|--read-only\t\tread only\n"
	       "\t-m|--multi-file\t\tmultiple file\n"
	       "\t-c|--copy-on-write\tcopy on write\n"
	       "\t-l|--authorize-file\tfile with list of hosts that are allowed to\n\t\t\t\tconnect.\n"
	       "\t-a|--idle-time\t\tmaximum idle seconds; server terminates when\n\t\t\t\tidle time exceeded\n\n"
	       "\tif port is set to 0, stdin is used (for running from inetd)\n"
	       "\tif file_to_export contains '%%s', it is substituted with the IP\n"
	       "\t\taddress of the machine trying to connect\n" );
}