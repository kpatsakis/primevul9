SERVER* cmdline(int argc, char *argv[]) {
	int i=0;
	int nonspecial=0;
	int c;
	struct option long_options[] = {
		{"read-only", no_argument, NULL, 'r'},
		{"multi-file", no_argument, NULL, 'm'},
		{"copy-on-write", no_argument, NULL, 'c'},
		{"authorize-file", required_argument, NULL, 'l'},
		{"idle-time", required_argument, NULL, 'a'},
		{0,0,0,0}
	};
	SERVER *serve;
	off_t es;
	size_t last;
	char suffix;

	serve=g_malloc(sizeof(SERVER));
	serve->hunksize=OFFT_MAX;
	while((c=getopt_long(argc, argv, "-a:cl:mr", long_options, &i))>=0) {
		switch (c) {
		case 1:
			switch(nonspecial++) {
			case 0:
				serve->port=strtol(optarg, NULL, 0);
				break;
			case 1:
				serve->exportname = g_strdup(optarg);
				if(serve->exportname[0] != '/') {
					fprintf(stderr, "E: The to be exported file needs to be an absolute filename!\n");
					exit(EXIT_FAILURE);
				}
				break;
			case 2:
				last=strlen(optarg)-1;
				suffix=optarg[last];
				if (suffix == 'k' || suffix == 'K' ||
				    suffix == 'm' || suffix == 'M')
					optarg[last] = '\0';
				es = (off_t)atol(optarg);
				switch (suffix) {
					case 'm':
					case 'M':  es <<= 10;
					case 'k':
					case 'K':  es <<= 10;
					default :  break;
				}
				serve->expected_size = es;
				break;
			}
			break;
		case 'r':
			serve->flags |= F_READONLY;
			break;
		case 'm':
			serve->flags |= F_MULTIFILE;
			serve->hunksize = 1*GIGA;
			serve->authname = default_authname;
			break;
		case 'c': 
			serve->flags |=F_COPYONWRITE;
		        break;
		case 'l':
			serve->authname=optarg;
			break;
		case 'a': 
			serve->timeout=strtol(optarg, NULL, 0);
			break;
		default:
			usage();
			exit(0);
			break;
		}
	}
	/* What's left: the port to export, the name of the to be exported
	 * file, and, optionally, the size of the file, in that order. */
	if(nonspecial<2) {
		usage();
		exit(EXIT_FAILURE);
	}
	return serve;
}