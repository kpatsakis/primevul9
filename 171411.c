
static void ql3xxx_remove(struct pci_dev *pdev)
{
	struct net_device *ndev = pci_get_drvdata(pdev);
	struct ql3_adapter *qdev = netdev_priv(ndev);

	unregister_netdev(ndev);

	ql_disable_interrupts(qdev);

	if (qdev->workqueue) {
		cancel_delayed_work(&qdev->reset_work);
		cancel_delayed_work(&qdev->tx_timeout_work);
		destroy_workqueue(qdev->workqueue);
		qdev->workqueue = NULL;
	}

	iounmap(qdev->mem_map_registers);
	pci_release_regions(pdev);
	free_netdev(ndev);