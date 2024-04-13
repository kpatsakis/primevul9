u16 capi20_register(struct capi20_appl *ap)
{
	int i;
	u16 applid;

	DBG("");

	if (ap->rparam.datablklen < 128)
		return CAPI_LOGBLKSIZETOSMALL;

	ap->nrecvctlpkt = 0;
	ap->nrecvdatapkt = 0;
	ap->nsentctlpkt = 0;
	ap->nsentdatapkt = 0;
	mutex_init(&ap->recv_mtx);
	skb_queue_head_init(&ap->recv_queue);
	INIT_WORK(&ap->recv_work, recv_handler);
	ap->release_in_progress = 0;

	mutex_lock(&capi_controller_lock);

	for (applid = 1; applid <= CAPI_MAXAPPL; applid++) {
		if (capi_applications[applid - 1] == NULL)
			break;
	}
	if (applid > CAPI_MAXAPPL) {
		mutex_unlock(&capi_controller_lock);
		return CAPI_TOOMANYAPPLS;
	}

	ap->applid = applid;
	capi_applications[applid - 1] = ap;

	for (i = 0; i < CAPI_MAXCONTR; i++) {
		if (!capi_controller[i] ||
		    capi_controller[i]->state != CAPI_CTR_RUNNING)
			continue;
		register_appl(capi_controller[i], applid, &ap->rparam);
	}

	mutex_unlock(&capi_controller_lock);

	if (showcapimsgs & 1) {
		printk(KERN_DEBUG "kcapi: appl %d up\n", applid);
	}

	return CAPI_NOERROR;
}