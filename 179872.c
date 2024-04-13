static void mwifiex_pcie_shutdown(struct pci_dev *pdev)
{
	mwifiex_pcie_remove(pdev);

	return;
}