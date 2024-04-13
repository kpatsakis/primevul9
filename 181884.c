u16 capi20_release(struct capi20_appl *ap)
{
	int i;

	DBG("applid %#x", ap->applid);

	mutex_lock(&capi_controller_lock);

	ap->release_in_progress = 1;
	capi_applications[ap->applid - 1] = NULL;

	synchronize_rcu();

	for (i = 0; i < CAPI_MAXCONTR; i++) {
		if (!capi_controller[i] ||
		    capi_controller[i]->state != CAPI_CTR_RUNNING)
			continue;
		release_appl(capi_controller[i], ap->applid);
	}

	mutex_unlock(&capi_controller_lock);

	flush_workqueue(kcapi_wq);
	skb_queue_purge(&ap->recv_queue);

	if (showcapimsgs & 1) {
		printk(KERN_DEBUG "kcapi: appl %d down\n", ap->applid);
	}

	return CAPI_NOERROR;
}