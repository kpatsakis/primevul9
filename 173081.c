static void bnx2x_remove_one(struct pci_dev *pdev)
{
	struct net_device *dev = pci_get_drvdata(pdev);
	struct bnx2x *bp;

	if (!dev) {
		dev_err(&pdev->dev, "BAD net device from bnx2x_init_one\n");
		return;
	}
	bp = netdev_priv(dev);

	__bnx2x_remove(pdev, dev, bp, true);
}