void kcapi_exit(void)
{
	kcapi_proc_exit();

	cdebug_exit();
	destroy_workqueue(kcapi_wq);
}