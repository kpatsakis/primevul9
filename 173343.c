u8 bnx2x_is_pcie_pending(struct pci_dev *dev)
{
	u16 status;

	pcie_capability_read_word(dev, PCI_EXP_DEVSTA, &status);
	return status & PCI_EXP_DEVSTA_TRPND;
}