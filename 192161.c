int __init mcheck_init(void)
{
	mcheck_intel_therm_init();
	mce_register_decode_chain(&first_nb);
	mce_register_decode_chain(&mce_srao_nb);
	mce_register_decode_chain(&mce_default_nb);
	mcheck_vendor_init_severity();

	INIT_WORK(&mce_work, mce_gen_pool_process);
	init_irq_work(&mce_irq_work, mce_irq_work_cb);

	return 0;
}