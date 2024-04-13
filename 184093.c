int realMain(int argc, char **argv)
{	
	DEFiRet;

	register uchar *p;
	int ch;
	struct hostent *hent;
	extern int optind;
	extern char *optarg;
	int bEOptionWasGiven = 0;
	int bImUxSockLoaded = 0; /* already generated a $ModLoad imuxsock? */
	int iHelperUOpt;
	int bChDirRoot = 1; /* change the current working directory to "/"? */
	char *arg;	/* for command line option processing */
	uchar legacyConfLine[80];
	uchar *LocalHostName;
	uchar *LocalDomain;
	uchar *LocalFQDNName;

	/* first, parse the command line options. We do not carry out any actual work, just
	 * see what we should do. This relieves us from certain anomalies and we can process
	 * the parameters down below in the correct order. For example, we must know the
	 * value of -M before we can do the init, but at the same time we need to have
	 * the base classes init before we can process most of the options. Now, with the
	 * split of functionality, this is no longer a problem. Thanks to varmofekoj for
	 * suggesting this algo.
	 * Note: where we just need to set some flags and can do so without knowledge
	 * of other options, we do this during the inital option processing. With later
	 * versions (if a dependency on -c option is introduced), we must move that code
	 * to other places, but I think it is quite appropriate and saves code to do this
	 * only when actually neeeded. 
	 * rgerhards, 2008-04-04
	 */
	while((ch = getopt(argc, argv, "46a:Ac:def:g:hi:l:m:M:nN:op:qQr::s:t:T:u:vwx")) != EOF) {
		switch((char)ch) {
                case '4':
                case '6':
                case 'A':
                case 'a':
		case 'f': /* configuration file */
		case 'h':
		case 'i': /* pid file name */
		case 'l':
		case 'm': /* mark interval */
		case 'n': /* don't fork */
		case 'N': /* enable config verify mode */
                case 'o':
                case 'p':
		case 'q': /* add hostname if DNS resolving has failed */
		case 'Q': /* dont resolve hostnames in ACL to IPs */
		case 's':
		case 'T': /* chroot on startup (primarily for testing) */
		case 'u': /* misc user settings */
		case 'w': /* disable disallowed host warnings */
		case 'x': /* disable dns for remote messages */
			CHKiRet(bufOptAdd(ch, optarg));
			break;
		case 'c':		/* compatibility mode */
			iCompatibilityMode = atoi(optarg);
			break;
		case 'd': /* debug - must be handled now, so that debug is active during init! */
			debugging_on = 1;
			Debug = 1;
			break;
		case 'e':		/* log every message (no repeat message supression) */
			fprintf(stderr, "note: -e option is no longer supported, every message is now logged by default\n");
			bEOptionWasGiven = 1;
			break;
		case 'g':		/* enable tcp gssapi logging */
#if defined(SYSLOG_INET) && defined(USE_GSSAPI)
			CHKiRet(bufOptAdd('g', optarg));
#else
			fprintf(stderr, "rsyslogd: -g not valid - not compiled with gssapi support");
#endif
			break;
		case 'M': /* default module load path -- this MUST be carried out immediately! */
			glblModPath = (uchar*) optarg;
			break;
		case 'r':		/* accept remote messages */
#ifdef SYSLOG_INET
			CHKiRet(bufOptAdd(ch, optarg));
#else
			fprintf(stderr, "rsyslogd: -r not valid - not compiled with network support\n");
#endif
			break;
		case 't':		/* enable tcp logging */
#ifdef SYSLOG_INET
			CHKiRet(bufOptAdd(ch, optarg));
#else
			fprintf(stderr, "rsyslogd: -t not valid - not compiled with network support\n");
#endif
			break;
		case 'v': /* MUST be carried out immediately! */
			printVersion();
			exit(0); /* exit for -v option - so this is a "good one" */
		case '?':              
		default:
			usage();
		}
	}

	if ((argc -= optind))
		usage();

	DBGPRINTF("rsyslogd %s startup, compatibility mode %d, module path '%s'\n",
		  VERSION, iCompatibilityMode, glblModPath == NULL ? "" : (char*)glblModPath);

	/* we are done with the initial option parsing and processing. Now we init the system. */

	ppid = getpid();

	CHKiRet_Hdlr(InitGlobalClasses()) {
		fprintf(stderr, "rsyslogd initializiation failed - global classes could not be initialized.\n"
				"Did you do a \"make install\"?\n"
				"Suggested action: run rsyslogd with -d -n options to see what exactly "
				"fails.\n");
		FINALIZE;
	}

	/* doing some core initializations */

	/* we need to create the inputName property (only once during our lifetime) */
	CHKiRet(prop.Construct(&pInternalInputName));
	CHKiRet(prop.SetString(pInternalInputName, UCHAR_CONSTANT("rsyslogd"), sizeof("rsyslgod") - 1));
	CHKiRet(prop.ConstructFinalize(pInternalInputName));

	CHKiRet(prop.Construct(&pLocalHostIP));
	CHKiRet(prop.SetString(pLocalHostIP, UCHAR_CONSTANT("127.0.0.1"), sizeof("127.0.0.1") - 1));
	CHKiRet(prop.ConstructFinalize(pLocalHostIP));

	/* get our host and domain names - we need to do this early as we may emit
	 * error log messages, which need the correct hostname. -- rgerhards, 2008-04-04
	 */
	net.getLocalHostname(&LocalFQDNName);
	CHKmalloc(LocalHostName = (uchar*) strdup((char*)LocalFQDNName));
	glbl.SetLocalFQDNName(LocalFQDNName); /* set the FQDN before we modify it */
	if((p = (uchar*)strchr((char*)LocalHostName, '.'))) {
		*p++ = '\0';
		LocalDomain = p;
	} else {
		LocalDomain = (uchar*)"";

		/* It's not clearly defined whether gethostname()
		 * should return the simple hostname or the fqdn. A
		 * good piece of software should be aware of both and
		 * we want to distribute good software.  Joey
		 *
		 * Good software also always checks its return values...
		 * If syslogd starts up before DNS is up & /etc/hosts
		 * doesn't have LocalHostName listed, gethostbyname will
		 * return NULL. 
		 */
		/* TODO: gethostbyname() is not thread-safe, but replacing it is
		 * not urgent as we do not run on multiple threads here. rgerhards, 2007-09-25
		 */
		hent = gethostbyname((char*)LocalHostName);
		if(hent) {
			free(LocalHostName);
			CHKmalloc(LocalHostName = (uchar*)strdup(hent->h_name));
				
			if((p = (uchar*)strchr((char*)LocalHostName, '.')))
			{
				*p++ = '\0';
				LocalDomain = p;
			}
		}
	}

	/* Convert to lower case to recognize the correct domain laterly */
	for(p = LocalDomain ; *p ; p++)
		*p = (char)tolower((int)*p);
	
	/* we now have our hostname and can set it inside the global vars.
	 * TODO: think if all of this would better be a runtime function
	 * rgerhards, 2008-04-17
	 */
	glbl.SetLocalHostName(LocalHostName);
	glbl.SetLocalDomain(LocalDomain);
	glbl.GenerateLocalHostNameProperty(); /* must be redone after conf processing, FQDN setting may have changed */

	/* initialize the objects */
	if((iRet = modInitIminternal()) != RS_RET_OK) {
		fprintf(stderr, "fatal error: could not initialize errbuf object (error code %d).\n",
			iRet);
		exit(1); /* "good" exit, leaving at init for fatal error */
	}

	if((iRet = loadBuildInModules()) != RS_RET_OK) {
		fprintf(stderr, "fatal error: could not activate built-in modules. Error code %d.\n",
			iRet);
		exit(1); /* "good" exit, leaving at init for fatal error */
	}

	/* END core initializations - we now come back to carrying out command line options*/

	while((iRet = bufOptRemove(&ch, &arg)) == RS_RET_OK) {
		DBGPRINTF("deque option %c, optarg '%s'\n", ch, (arg == NULL) ? "" : arg);
		switch((char)ch) {
                case '4':
	                glbl.SetDefPFFamily(PF_INET);
                        break;
                case '6':
                        glbl.SetDefPFFamily(PF_INET6);
                        break;
                case 'A':
                        send_to_all++;
                        break;
                case 'a':
			if(iCompatibilityMode < 3) {
				if(!bImUxSockLoaded) {
					legacyOptsEnq((uchar *) "ModLoad imuxsock");
					bImUxSockLoaded = 1;
				}
				snprintf((char *) legacyConfLine, sizeof(legacyConfLine), "addunixlistensocket %s", arg);
				legacyOptsEnq(legacyConfLine);
			} else {
				fprintf(stderr, "error -a is no longer supported, use module imuxsock instead");
			}
                        break;
		case 'f':		/* configuration file */
			ConfFile = (uchar*) arg;
			break;
		case 'g':		/* enable tcp gssapi logging */
			if(iCompatibilityMode < 3) {
				legacyOptsParseTCP(ch, arg);
			} else
				fprintf(stderr,	"-g option only supported in compatibility modes 0 to 2 - ignored\n");
			break;
		case 'h':
			if(iCompatibilityMode < 3) {
				errmsg.LogError(0, NO_ERRCODE, "WARNING: -h option is no longer supported - ignored");
			} else {
				usage(); /* for v3 and above, it simply is an error */
			}
			break;
		case 'i':		/* pid file name */
			PidFile = arg;
			break;
		case 'l':
			if(glbl.GetLocalHosts() != NULL) {
				fprintf (stderr, "rsyslogd: Only one -l argument allowed, the first one is taken.\n");
			} else {
				glbl.SetLocalHosts(crunch_list(arg));
			}
			break;
		case 'm':		/* mark interval */
			if(iCompatibilityMode < 3) {
				MarkInterval = atoi(arg) * 60;
			} else
				fprintf(stderr,
					"-m option only supported in compatibility modes 0 to 2 - ignored\n");
			break;
		case 'n':		/* don't fork */
			NoFork = 1;
			break;
		case 'N':		/* enable config verify mode */
			iConfigVerify = atoi(arg);
			break;
                case 'o':
			if(iCompatibilityMode < 3) {
				if(!bImUxSockLoaded) {
					legacyOptsEnq((uchar *) "ModLoad imuxsock");
					bImUxSockLoaded = 1;
				}
				legacyOptsEnq((uchar *) "OmitLocalLogging");
			} else {
				fprintf(stderr, "error -o is no longer supported, use module imuxsock instead");
			}
                        break;
                case 'p':
			if(iCompatibilityMode < 3) {
				if(!bImUxSockLoaded) {
					legacyOptsEnq((uchar *) "ModLoad imuxsock");
					bImUxSockLoaded = 1;
				}
				snprintf((char *) legacyConfLine, sizeof(legacyConfLine), "SystemLogSocketName %s", arg);
				legacyOptsEnq(legacyConfLine);
			} else {
				fprintf(stderr, "error -p is no longer supported, use module imuxsock instead");
			}
			break;
		case 'q':               /* add hostname if DNS resolving has failed */
		        *(net.pACLAddHostnameOnFail) = 1;
		        break;
		case 'Q':               /* dont resolve hostnames in ACL to IPs */
		        *(net.pACLDontResolve) = 1;
		        break;
		case 'r':		/* accept remote messages */
			if(iCompatibilityMode < 3) {
				legacyOptsEnq((uchar *) "ModLoad imudp");
				snprintf((char *) legacyConfLine, sizeof(legacyConfLine), "UDPServerRun %s", arg);
				legacyOptsEnq(legacyConfLine);
			} else
				fprintf(stderr, "-r option only supported in compatibility modes 0 to 2 - ignored\n");
			break;
		case 's':
			if(glbl.GetStripDomains() != NULL) {
				fprintf (stderr, "rsyslogd: Only one -s argument allowed, the first one is taken.\n");
			} else {
				glbl.SetStripDomains(crunch_list(arg));
			}
			break;
		case 't':		/* enable tcp logging */
			if(iCompatibilityMode < 3) {
				legacyOptsParseTCP(ch, arg);
			} else
				fprintf(stderr,	"-t option only supported in compatibility modes 0 to 2 - ignored\n");
			break;
		case 'T':/* chroot() immediately at program startup, but only for testing, NOT security yet */
			if(chroot(arg) != 0) {
				perror("chroot");
				exit(1);
			}
			break;
		case 'u':		/* misc user settings */
			iHelperUOpt = atoi(arg);
			if(iHelperUOpt & 0x01)
				bParseHOSTNAMEandTAG = 0;
			if(iHelperUOpt & 0x02)
				bChDirRoot = 0;
			break;
		case 'w':		/* disable disallowed host warnigs */
			glbl.SetOption_DisallowWarning(0);
			break;
		case 'x':		/* disable dns for remote messages */
			glbl.SetDisableDNS(1);
			break;
		case '?':              
		default:
			usage();
		}
	}

	if(iRet != RS_RET_END_OF_LINKEDLIST)
		FINALIZE;

	if(iConfigVerify) {
		fprintf(stderr, "rsyslogd: version %s, config validation run (level %d), master config %s\n",
			VERSION, iConfigVerify, ConfFile);
	}

	if(bChDirRoot) {
		if(chdir("/") != 0)
			fprintf(stderr, "Can not do 'cd /' - still trying to run\n");
	}


	/* process compatibility mode settings */
	if(iCompatibilityMode < 4) {
		errmsg.LogError(0, NO_ERRCODE, "WARNING: rsyslogd is running in compatibility mode. Automatically "
		                            "generated config directives may interfer with your rsyslog.conf settings. "
					    "We suggest upgrading your config and adding -c4 as the first "
					    "rsyslogd option.");
	}

	if(iCompatibilityMode < 3) {
		if(MarkInterval > 0) {
			legacyOptsEnq((uchar *) "ModLoad immark");
			snprintf((char *) legacyConfLine, sizeof(legacyConfLine), "MarkMessagePeriod %d", MarkInterval);
			legacyOptsEnq(legacyConfLine);
		}
		if(!bImUxSockLoaded) {
			legacyOptsEnq((uchar *) "ModLoad imuxsock");
		}
	}

	if(bEOptionWasGiven && iCompatibilityMode < 3) {
		errmsg.LogError(0, NO_ERRCODE, "WARNING: \"message repeated n times\" feature MUST be turned on in "
					    "rsyslog.conf - CURRENTLY EVERY MESSAGE WILL BE LOGGED. Visit "
					    "http://www.rsyslog.com/rptdmsgreduction to learn "
					    "more and cast your vote if you want us to keep this feature.");
	}

	if(!iConfigVerify)
		CHKiRet(doGlblProcessInit());

	/* re-generate local host name property, as the config may have changed our FQDN settings */
	glbl.GenerateLocalHostNameProperty();

	CHKiRet(mainThread());

	/* do any de-init's that need to be done AFTER this comment */

	die(bFinished);
	
	thrdExit();

finalize_it:
	if(iRet == RS_RET_VALIDATION_RUN) {
		fprintf(stderr, "rsyslogd: End of config validation run. Bye.\n");
	} else if(iRet != RS_RET_OK) {
		fprintf(stderr, "rsyslogd run failed with error %d (see rsyslog.h "
				"or try http://www.rsyslog.com/e/%d to learn what that number means)\n", iRet, iRet*-1);
	}

	ENDfunc
	return 0;
}