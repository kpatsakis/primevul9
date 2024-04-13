static void __exit fsl_hypervisor_exit(void)
{
	struct doorbell_isr *dbisr, *n;

	list_for_each_entry_safe(dbisr, n, &isr_list, list) {
		free_irq(dbisr->irq, dbisr);
		list_del(&dbisr->list);
		kfree(dbisr);
	}

	misc_deregister(&fsl_hv_misc_dev);
}