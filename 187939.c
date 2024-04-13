static int dwc3_byt_enable_ulpi_refclock(struct pci_dev *pci)
{
	void __iomem	*reg;
	u32		value;

	reg = pcim_iomap(pci, GP_RWBAR, 0);
	if (!reg)
		return -ENOMEM;

	value = readl(reg + GP_RWREG1);
	if (!(value & GP_RWREG1_ULPI_REFCLK_DISABLE))
		goto unmap; /* ULPI refclk already enabled */

	value &= ~GP_RWREG1_ULPI_REFCLK_DISABLE;
	writel(value, reg + GP_RWREG1);
	/* This comes from the Intel Android x86 tree w/o any explanation */
	msleep(100);
unmap:
	pcim_iounmap(pci, reg);
	return 0;
}