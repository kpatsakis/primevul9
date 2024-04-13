u16 capi20_put_message(struct capi20_appl *ap, struct sk_buff *skb)
{
	struct capi_ctr *ctr;
	int showctl = 0;
	u8 cmd, subcmd;

	DBG("applid %#x", ap->applid);

	if (ncontrollers == 0)
		return CAPI_REGNOTINSTALLED;
	if ((ap->applid == 0) || ap->release_in_progress)
		return CAPI_ILLAPPNR;
	if (skb->len < 12
	    || !capi_cmd_valid(CAPIMSG_COMMAND(skb->data))
	    || !capi_subcmd_valid(CAPIMSG_SUBCOMMAND(skb->data)))
		return CAPI_ILLCMDORSUBCMDORMSGTOSMALL;

	/*
	 * The controller reference is protected by the existence of the
	 * application passed to us. We assume that the caller properly
	 * synchronizes this service with capi20_release.
	 */
	ctr = get_capi_ctr_by_nr(CAPIMSG_CONTROLLER(skb->data));
	if (!ctr || ctr->state != CAPI_CTR_RUNNING)
		return CAPI_REGNOTINSTALLED;
	if (ctr->blocked)
		return CAPI_SENDQUEUEFULL;

	cmd = CAPIMSG_COMMAND(skb->data);
	subcmd = CAPIMSG_SUBCOMMAND(skb->data);

	if (cmd == CAPI_DATA_B3 && subcmd == CAPI_REQ) {
		ctr->nsentdatapkt++;
		ap->nsentdatapkt++;
		if (ctr->traceflag > 2)
			showctl |= 2;
	} else {
		ctr->nsentctlpkt++;
		ap->nsentctlpkt++;
		if (ctr->traceflag)
			showctl |= 2;
	}
	showctl |= (ctr->traceflag & 1);
	if (showctl & 2) {
		if (showctl & 1) {
			printk(KERN_DEBUG "kcapi: put [%03d] id#%d %s len=%u\n",
			       CAPIMSG_CONTROLLER(skb->data),
			       CAPIMSG_APPID(skb->data),
			       capi_cmd2str(cmd, subcmd),
			       CAPIMSG_LEN(skb->data));
		} else {
			_cdebbuf *cdb = capi_message2str(skb->data);
			if (cdb) {
				printk(KERN_DEBUG "kcapi: put [%03d] %s\n",
				       CAPIMSG_CONTROLLER(skb->data),
				       cdb->buf);
				cdebbuf_free(cdb);
			} else
				printk(KERN_DEBUG "kcapi: put [%03d] id#%d %s len=%u cannot trace\n",
				       CAPIMSG_CONTROLLER(skb->data),
				       CAPIMSG_APPID(skb->data),
				       capi_cmd2str(cmd, subcmd),
				       CAPIMSG_LEN(skb->data));
		}
	}
	return ctr->send_message(ctr, skb);
}