u32 bnx2x_send_unload_req(struct bnx2x *bp, int unload_mode)
{
	u32 reset_code = 0;
	int port = BP_PORT(bp);

	/* Select the UNLOAD request mode */
	if (unload_mode == UNLOAD_NORMAL)
		reset_code = DRV_MSG_CODE_UNLOAD_REQ_WOL_DIS;

	else if (bp->flags & NO_WOL_FLAG)
		reset_code = DRV_MSG_CODE_UNLOAD_REQ_WOL_MCP;

	else if (bp->wol) {
		u32 emac_base = port ? GRCBASE_EMAC1 : GRCBASE_EMAC0;
		u8 *mac_addr = bp->dev->dev_addr;
		struct pci_dev *pdev = bp->pdev;
		u32 val;
		u16 pmc;

		/* The mac address is written to entries 1-4 to
		 * preserve entry 0 which is used by the PMF
		 */
		u8 entry = (BP_VN(bp) + 1)*8;

		val = (mac_addr[0] << 8) | mac_addr[1];
		EMAC_WR(bp, EMAC_REG_EMAC_MAC_MATCH + entry, val);

		val = (mac_addr[2] << 24) | (mac_addr[3] << 16) |
		      (mac_addr[4] << 8) | mac_addr[5];
		EMAC_WR(bp, EMAC_REG_EMAC_MAC_MATCH + entry + 4, val);

		/* Enable the PME and clear the status */
		pci_read_config_word(pdev, pdev->pm_cap + PCI_PM_CTRL, &pmc);
		pmc |= PCI_PM_CTRL_PME_ENABLE | PCI_PM_CTRL_PME_STATUS;
		pci_write_config_word(pdev, pdev->pm_cap + PCI_PM_CTRL, pmc);

		reset_code = DRV_MSG_CODE_UNLOAD_REQ_WOL_EN;

	} else
		reset_code = DRV_MSG_CODE_UNLOAD_REQ_WOL_DIS;

	/* Send the request to the MCP */
	if (!BP_NOMCP(bp))
		reset_code = bnx2x_fw_command(bp, reset_code, 0);
	else {
		int path = BP_PATH(bp);

		DP(NETIF_MSG_IFDOWN, "NO MCP - load counts[%d]      %d, %d, %d\n",
		   path, bnx2x_load_count[path][0], bnx2x_load_count[path][1],
		   bnx2x_load_count[path][2]);
		bnx2x_load_count[path][0]--;
		bnx2x_load_count[path][1 + port]--;
		DP(NETIF_MSG_IFDOWN, "NO MCP - new load counts[%d]  %d, %d, %d\n",
		   path, bnx2x_load_count[path][0], bnx2x_load_count[path][1],
		   bnx2x_load_count[path][2]);
		if (bnx2x_load_count[path][0] == 0)
			reset_code = FW_MSG_CODE_DRV_UNLOAD_COMMON;
		else if (bnx2x_load_count[path][1 + port] == 0)
			reset_code = FW_MSG_CODE_DRV_UNLOAD_PORT;
		else
			reset_code = FW_MSG_CODE_DRV_UNLOAD_FUNCTION;
	}

	return reset_code;
}