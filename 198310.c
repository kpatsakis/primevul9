static int sctp_verify_ext_param(struct net *net, union sctp_params param)
{
	__u16 num_ext = ntohs(param.p->length) - sizeof(struct sctp_paramhdr);
	int have_asconf = 0;
	int have_auth = 0;
	int i;

	for (i = 0; i < num_ext; i++) {
		switch (param.ext->chunks[i]) {
		case SCTP_CID_AUTH:
			have_auth = 1;
			break;
		case SCTP_CID_ASCONF:
		case SCTP_CID_ASCONF_ACK:
			have_asconf = 1;
			break;
		}
	}

	/* ADD-IP Security: The draft requires us to ABORT or ignore the
	 * INIT/INIT-ACK if ADD-IP is listed, but AUTH is not.  Do this
	 * only if ADD-IP is turned on and we are not backward-compatible
	 * mode.
	 */
	if (net->sctp.addip_noauth)
		return 1;

	if (net->sctp.addip_enable && !have_auth && have_asconf)
		return 0;

	return 1;
}