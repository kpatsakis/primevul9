void capi_ctr_ready(struct capi_ctr *ctr)
{
	printk(KERN_NOTICE "kcapi: controller [%03d] \"%s\" ready.\n",
	       ctr->cnr, ctr->name);

	notify_push(CAPICTR_UP, ctr->cnr);
}