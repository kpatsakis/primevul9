void capi_ctr_handle_message(struct capi_ctr *ctr, u16 appl,
			     struct sk_buff *skb)
{
	struct capi20_appl *ap;
	int showctl = 0;
	u8 cmd, subcmd;
	_cdebbuf *cdb;

	if (ctr->state != CAPI_CTR_RUNNING) {
		cdb = capi_message2str(skb->data);
		if (cdb) {
			printk(KERN_INFO "kcapi: controller [%03d] not active, got: %s",
			       ctr->cnr, cdb->buf);
			cdebbuf_free(cdb);
		} else
			printk(KERN_INFO "kcapi: controller [%03d] not active, cannot trace\n",
			       ctr->cnr);
		goto error;
	}

	cmd = CAPIMSG_COMMAND(skb->data);
	subcmd = CAPIMSG_SUBCOMMAND(skb->data);
	if (cmd == CAPI_DATA_B3 && subcmd == CAPI_IND) {
		ctr->nrecvdatapkt++;
		if (ctr->traceflag > 2)
			showctl |= 2;
	} else {
		ctr->nrecvctlpkt++;
		if (ctr->traceflag)
			showctl |= 2;
	}
	showctl |= (ctr->traceflag & 1);
	if (showctl & 2) {
		if (showctl & 1) {
			printk(KERN_DEBUG "kcapi: got [%03d] id#%d %s len=%u\n",
			       ctr->cnr, CAPIMSG_APPID(skb->data),
			       capi_cmd2str(cmd, subcmd),
			       CAPIMSG_LEN(skb->data));
		} else {
			cdb = capi_message2str(skb->data);
			if (cdb) {
				printk(KERN_DEBUG "kcapi: got [%03d] %s\n",
				       ctr->cnr, cdb->buf);
				cdebbuf_free(cdb);
			} else
				printk(KERN_DEBUG "kcapi: got [%03d] id#%d %s len=%u, cannot trace\n",
				       ctr->cnr, CAPIMSG_APPID(skb->data),
				       capi_cmd2str(cmd, subcmd),
				       CAPIMSG_LEN(skb->data));
		}

	}

	rcu_read_lock();
	ap = get_capi_appl_by_nr(CAPIMSG_APPID(skb->data));
	if (!ap) {
		rcu_read_unlock();
		cdb = capi_message2str(skb->data);
		if (cdb) {
			printk(KERN_ERR "kcapi: handle_message: applid %d state released (%s)\n",
			       CAPIMSG_APPID(skb->data), cdb->buf);
			cdebbuf_free(cdb);
		} else
			printk(KERN_ERR "kcapi: handle_message: applid %d state released (%s) cannot trace\n",
			       CAPIMSG_APPID(skb->data),
			       capi_cmd2str(cmd, subcmd));
		goto error;
	}
	skb_queue_tail(&ap->recv_queue, skb);
	queue_work(kcapi_wq, &ap->recv_work);
	rcu_read_unlock();

	return;

error:
	kfree_skb(skb);
}