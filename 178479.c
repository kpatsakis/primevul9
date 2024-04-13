static void __exit hsm_exit(void)
{
	acrn_ioreq_intr_remove();
	misc_deregister(&acrn_dev);
}