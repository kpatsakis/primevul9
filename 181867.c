void capi_ctr_down(struct capi_ctr *ctr)
{
	printk(KERN_NOTICE "kcapi: controller [%03d] down.\n", ctr->cnr);

	notify_push(CAPICTR_DOWN, ctr->cnr);
}