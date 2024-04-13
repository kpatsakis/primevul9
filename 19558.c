static void iwl_trans_pcie_removal_wk(struct work_struct *wk)
{
	struct iwl_trans_pcie_removal *removal =
		container_of(wk, struct iwl_trans_pcie_removal, work);
	struct pci_dev *pdev = removal->pdev;
	static char *prop[] = {"EVENT=INACCESSIBLE", NULL};

	dev_err(&pdev->dev, "Device gone - attempting removal\n");
	kobject_uevent_env(&pdev->dev.kobj, KOBJ_CHANGE, prop);
	pci_lock_rescan_remove();
	pci_dev_put(pdev);
	pci_stop_and_remove_bus_device(pdev);
	pci_unlock_rescan_remove();

	kfree(removal);
	module_put(THIS_MODULE);
}