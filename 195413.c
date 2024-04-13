mptctl_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
	MPT_ADAPTER *ioc = pci_get_drvdata(pdev);

	mutex_init(&ioc->ioctl_cmds.mutex);
	init_completion(&ioc->ioctl_cmds.done);
	return 0;
}