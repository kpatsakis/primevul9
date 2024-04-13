int attach_capi_ctr(struct capi_ctr *ctr)
{
	int i;

	mutex_lock(&capi_controller_lock);

	for (i = 0; i < CAPI_MAXCONTR; i++) {
		if (!capi_controller[i])
			break;
	}
	if (i == CAPI_MAXCONTR) {
		mutex_unlock(&capi_controller_lock);
		printk(KERN_ERR "kcapi: out of controller slots\n");
		return -EBUSY;
	}
	capi_controller[i] = ctr;

	ctr->nrecvctlpkt = 0;
	ctr->nrecvdatapkt = 0;
	ctr->nsentctlpkt = 0;
	ctr->nsentdatapkt = 0;
	ctr->cnr = i + 1;
	ctr->state = CAPI_CTR_DETECTED;
	ctr->blocked = 0;
	ctr->traceflag = showcapimsgs;

	sprintf(ctr->procfn, "capi/controllers/%d", ctr->cnr);
	ctr->procent = proc_create_single_data(ctr->procfn, 0, NULL,
			ctr->proc_show, ctr);

	ncontrollers++;

	mutex_unlock(&capi_controller_lock);

	printk(KERN_NOTICE "kcapi: controller [%03d]: %s attached\n",
	       ctr->cnr, ctr->name);
	return 0;
}