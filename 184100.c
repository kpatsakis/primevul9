void legacyOptsParseTCP(char ch, char *arg)
{
	register int i;
	register char *pArg = arg;
	static char conflict = '\0';

	if((conflict == 'g' && ch == 't') || (conflict == 't' && ch == 'g')) {
		fprintf(stderr, "rsyslogd: If you want to use both -g and -t, use directives instead, -%c ignored.\n", ch);
		return;
	} else
		conflict = ch;

	/* extract port */
	i = 0;
	while(isdigit((int) *pArg))
		i = i * 10 + *pArg++ - '0';

	/* number of sessions */
	if(*pArg == '\0' || *pArg == ',') {
		if(ch == 't')
			legacyOptsEnq((uchar *) "ModLoad imtcp");
		else if(ch == 'g')
			legacyOptsEnq((uchar *) "ModLoad imgssapi");

		if(i >= 0 && i <= 65535) {
			uchar line[30];

			if(ch == 't') {
				snprintf((char *) line, sizeof(line), "InputTCPServerRun %d", i);
			} else if(ch == 'g') {
				snprintf((char *) line, sizeof(line), "InputGSSServerRun %d", i);
			}
			legacyOptsEnq(line);
		} else {
			if(ch == 't') {
				fprintf(stderr, "rsyslogd: Invalid TCP listen port %d - changed to 514.\n", i);
				legacyOptsEnq((uchar *) "InputTCPServerRun 514");
			} else if(ch == 'g') {
				fprintf(stderr, "rsyslogd: Invalid GSS listen port %d - changed to 514.\n", i);
				legacyOptsEnq((uchar *) "InputGSSServerRun 514");
			}
		}

		if(*pArg == ',') {
			++pArg;
			while(isspace((int) *pArg))
				++pArg;
			i = 0;
			while(isdigit((int) *pArg)) {
				i = i * 10 + *pArg++ - '0';
			}
			if(i > 0) {
				uchar line[30];

				snprintf((char *) line, sizeof(line), "InputTCPMaxSessions %d", i);
				legacyOptsEnq(line);
			} else {
				if(ch == 't') {
					fprintf(stderr,	"rsyslogd: TCP session max configured "
						"to %d [-t %s] - changing to 1.\n", i, arg);
					legacyOptsEnq((uchar *) "InputTCPMaxSessions 1");
				} else if (ch == 'g') {
					fprintf(stderr,	"rsyslogd: GSS session max configured "
						"to %d [-g %s] - changing to 1.\n", i, arg);
					legacyOptsEnq((uchar *) "InputTCPMaxSessions 1");
				}
			}
		}
	} else
		fprintf(stderr, "rsyslogd: Invalid -t %s command line option.\n", arg);
}