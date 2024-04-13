static void bnx2x_reg_wr_ind(struct bnx2x *bp, u32 addr, u32 val)
{
	pci_write_config_dword(bp->pdev, PCICFG_GRC_ADDRESS, addr);
	pci_write_config_dword(bp->pdev, PCICFG_GRC_DATA, val);
	pci_write_config_dword(bp->pdev, PCICFG_GRC_ADDRESS,
			       PCICFG_VENDOR_ID_OFFSET);
}