static void bnx2x_disable_pcie_error_reporting(struct bnx2x *bp)
{
	if (bp->flags & AER_ENABLED) {
		pci_disable_pcie_error_reporting(bp->pdev);
		bp->flags &= ~AER_ENABLED;
	}
}