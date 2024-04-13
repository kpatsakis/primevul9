static int __init i40e_init_module(void)
{
	pr_info("%s: %s - version %s\n", i40e_driver_name,
		i40e_driver_string, i40e_driver_version_str);
	pr_info("%s: %s\n", i40e_driver_name, i40e_copyright);

	/* There is no need to throttle the number of active tasks because
	 * each device limits its own task using a state bit for scheduling
	 * the service task, and the device tasks do not interfere with each
	 * other, so we don't set a max task limit. We must set WQ_MEM_RECLAIM
	 * since we need to be able to guarantee forward progress even under
	 * memory pressure.
	 */
	i40e_wq = alloc_workqueue("%s", WQ_MEM_RECLAIM, 0, i40e_driver_name);
	if (!i40e_wq) {
		pr_err("%s: Failed to create workqueue\n", i40e_driver_name);
		return -ENOMEM;
	}

	i40e_dbg_init();
	return pci_register_driver(&i40e_driver);
}