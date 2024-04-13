static void __exit i40e_exit_module(void)
{
	pci_unregister_driver(&i40e_driver);
	destroy_workqueue(i40e_wq);
	i40e_dbg_exit();
}