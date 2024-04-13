
static void ql_display_dev_info(struct net_device *ndev)
{
	struct ql3_adapter *qdev = netdev_priv(ndev);
	struct pci_dev *pdev = qdev->pdev;

	netdev_info(ndev,
		    "%s Adapter %d RevisionID %d found %s on PCI slot %d\n",
		    DRV_NAME, qdev->index, qdev->chip_rev_id,
		    qdev->device_id == QL3032_DEVICE_ID ? "QLA3032" : "QLA3022",
		    qdev->pci_slot);
	netdev_info(ndev, "%s Interface\n",
		test_bit(QL_LINK_OPTICAL, &qdev->flags) ? "OPTICAL" : "COPPER");

	/*
	 * Print PCI bus width/type.
	 */
	netdev_info(ndev, "Bus interface is %s %s\n",
		    ((qdev->pci_width == 64) ? "64-bit" : "32-bit"),
		    ((qdev->pci_x) ? "PCI-X" : "PCI"));

	netdev_info(ndev, "mem  IO base address adjusted = 0x%p\n",
		    qdev->mem_map_registers);
	netdev_info(ndev, "Interrupt number = %d\n", pdev->irq);

	netif_info(qdev, probe, ndev, "MAC address %pM\n", ndev->dev_addr);