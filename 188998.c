static void i40e_get_platform_mac_addr(struct pci_dev *pdev, struct i40e_pf *pf)
{
	if (eth_platform_get_mac_address(&pdev->dev, pf->hw.mac.addr))
		i40e_get_mac_addr(&pf->hw, pf->hw.mac.addr);
}