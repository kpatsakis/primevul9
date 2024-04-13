static int unix_accounting(void *instance, REQUEST *request)
{
	VALUE_PAIR	*vp;
	FILE		*fp;
	struct utmp	ut;
	time_t		t;
	char		buf[64];
	const char	*s;
	int		delay = 0;
	int		status = -1;
	int		nas_address = 0;
	int		framed_address = 0;
#ifdef USER_PROCESS
	int		protocol = -1;
#endif
	int		nas_port = 0;
	int		port_seen = 0;
	struct unix_instance *inst = (struct unix_instance *) instance;

	/*
	 *	No radwtmp.  Don't do anything.
	 */
	if (!inst->radwtmp) {
		RDEBUG2("No radwtmp file configured.  Ignoring accounting request.");
		return RLM_MODULE_NOOP;
	}

	if (request->packet->src_ipaddr.af != AF_INET) {
		RDEBUG2("IPv6 is not supported!");
		return RLM_MODULE_NOOP;
	}

	/*
	 *	Which type is this.
	 */
	if ((vp = pairfind(request->packet->vps, PW_ACCT_STATUS_TYPE, 0))==NULL) {
		RDEBUG("no Accounting-Status-Type attribute in request.");
		return RLM_MODULE_NOOP;
	}
	status = vp->vp_integer;

	/*
	 *	FIXME: handle PW_STATUS_ALIVE like 1.5.4.3 did.
	 */
	if (status != PW_STATUS_START &&
	    status != PW_STATUS_STOP)
		return RLM_MODULE_NOOP;

	/*
	 *	We're only interested in accounting messages
	 *	with a username in it.
	 */
	if (pairfind(request->packet->vps, PW_USER_NAME, 0) == NULL)
		return RLM_MODULE_NOOP;

	t = request->timestamp;
	memset(&ut, 0, sizeof(ut));

	/*
	 *	First, find the interesting attributes.
	 */
	for (vp = request->packet->vps; vp; vp = vp->next) {
		switch (vp->attribute) {
			case PW_USER_NAME:
				if (vp->length >= sizeof(ut.ut_name)) {
					memcpy(ut.ut_name, (char *)vp->vp_strvalue, sizeof(ut.ut_name));
				} else {
					strlcpy(ut.ut_name, (char *)vp->vp_strvalue, sizeof(ut.ut_name));
				}
				break;
			case PW_LOGIN_IP_HOST:
			case PW_FRAMED_IP_ADDRESS:
				framed_address = vp->vp_ipaddr;
				break;
#ifdef USER_PROCESS
			case PW_FRAMED_PROTOCOL:
				protocol = vp->vp_integer;
				break;
#endif
			case PW_NAS_IP_ADDRESS:
				nas_address = vp->vp_ipaddr;
				break;
			case PW_NAS_PORT:
				nas_port = vp->vp_integer;
				port_seen = 1;
				break;
			case PW_ACCT_DELAY_TIME:
				delay = vp->vp_ipaddr;
				break;
		}
	}

	/*
	 *	We don't store !root sessions, or sessions
	 *	where we didn't see a NAS-Port attribute.
	 */
	if (strncmp(ut.ut_name, "!root", sizeof(ut.ut_name)) == 0 || !port_seen)
		return RLM_MODULE_NOOP;

	/*
	 *	If we didn't find out the NAS address, use the
	 *	originator's IP address.
	 */
	if (nas_address == 0) {
		nas_address = request->packet->src_ipaddr.ipaddr.ip4addr.s_addr;
	}
	s = request->client->shortname;
	if (!s || s[0] == 0) s = uue(&(nas_address));

#ifdef __linux__
	/*
	 *	Linux has a field for the client address.
	 */
	ut.ut_addr = framed_address;
#endif
	/*
	 *	We use the tty field to store the terminal servers' port
	 *	and address so that the tty field is unique.
	 */
	snprintf(buf, sizeof(buf), "%03d:%s", nas_port, s);
	strlcpy(ut.ut_line, buf, sizeof(ut.ut_line));

	/*
	 *	We store the dynamic IP address in the hostname field.
	 */
#ifdef UT_HOSTSIZE
	if (framed_address) {
		ip_ntoa(buf, framed_address);
		strlcpy(ut.ut_host, buf, sizeof(ut.ut_host));
	}
#endif
#ifdef HAVE_UTMPX_H
	ut.ut_xtime = t- delay;
#else
	ut.ut_time = t - delay;
#endif
#ifdef USER_PROCESS
	/*
	 *	And we can use the ID field to store
	 *	the protocol.
	 */
	if (protocol == PW_PPP)
		strcpy(ut.ut_id, "P");
	else if (protocol == PW_SLIP)
		strcpy(ut.ut_id, "S");
	else
		strcpy(ut.ut_id, "T");
	ut.ut_type = status == PW_STATUS_STOP ? DEAD_PROCESS : USER_PROCESS;
#endif
	if (status == PW_STATUS_STOP)
		ut.ut_name[0] = 0;

	/*
	 *	Write a RADIUS wtmp log file.
	 *
	 *	Try to open the file if we can't, we don't write the
	 *	wtmp file. If we can try to write. If we fail,
	 *	return RLM_MODULE_FAIL ..
	 */
	if ((fp = fopen(inst->radwtmp, "a")) != NULL) {
		if ((fwrite(&ut, sizeof(ut), 1, fp)) != 1) {
			fclose(fp);
			return RLM_MODULE_FAIL;
		}
		fclose(fp);
	} else
		return RLM_MODULE_FAIL;

	return RLM_MODULE_OK;
}