int __init kcapi_init(void)
{
	int err;

	kcapi_wq = alloc_workqueue("kcapi", 0, 0);
	if (!kcapi_wq)
		return -ENOMEM;

	err = cdebug_init();
	if (err) {
		destroy_workqueue(kcapi_wq);
		return err;
	}

	kcapi_proc_init();
	return 0;
}