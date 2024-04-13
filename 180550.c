static int get_last_alias(struct pci_dev *pdev, u16 alias, void *opaque)
{
	*(u16 *)opaque = alias;
	return 0;
}