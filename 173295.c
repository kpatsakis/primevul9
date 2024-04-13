static int bnx2x_get_fc_npiv(struct net_device *dev,
			     struct cnic_fc_npiv_tbl *cnic_tbl)
{
	struct bnx2x *bp = netdev_priv(dev);
	struct bdn_fc_npiv_tbl *tbl = NULL;
	u32 offset, entries;
	int rc = -EINVAL;
	int i;

	if (!SHMEM2_HAS(bp, fc_npiv_nvram_tbl_addr[0]))
		goto out;

	DP(BNX2X_MSG_MCP, "About to read the FC-NPIV table\n");

	tbl = kmalloc(sizeof(*tbl), GFP_KERNEL);
	if (!tbl) {
		BNX2X_ERR("Failed to allocate fc_npiv table\n");
		goto out;
	}

	offset = SHMEM2_RD(bp, fc_npiv_nvram_tbl_addr[BP_PORT(bp)]);
	if (!offset) {
		DP(BNX2X_MSG_MCP, "No FC-NPIV in NVRAM\n");
		goto out;
	}
	DP(BNX2X_MSG_MCP, "Offset of FC-NPIV in NVRAM: %08x\n", offset);

	/* Read the table contents from nvram */
	if (bnx2x_nvram_read(bp, offset, (u8 *)tbl, sizeof(*tbl))) {
		BNX2X_ERR("Failed to read FC-NPIV table\n");
		goto out;
	}

	/* Since bnx2x_nvram_read() returns data in be32, we need to convert
	 * the number of entries back to cpu endianness.
	 */
	entries = tbl->fc_npiv_cfg.num_of_npiv;
	entries = (__force u32)be32_to_cpu((__force __be32)entries);
	tbl->fc_npiv_cfg.num_of_npiv = entries;

	if (!tbl->fc_npiv_cfg.num_of_npiv) {
		DP(BNX2X_MSG_MCP,
		   "No FC-NPIV table [valid, simply not present]\n");
		goto out;
	} else if (tbl->fc_npiv_cfg.num_of_npiv > MAX_NUMBER_NPIV) {
		BNX2X_ERR("FC-NPIV table with bad length 0x%08x\n",
			  tbl->fc_npiv_cfg.num_of_npiv);
		goto out;
	} else {
		DP(BNX2X_MSG_MCP, "Read 0x%08x entries from NVRAM\n",
		   tbl->fc_npiv_cfg.num_of_npiv);
	}

	/* Copy the data into cnic-provided struct */
	cnic_tbl->count = tbl->fc_npiv_cfg.num_of_npiv;
	for (i = 0; i < cnic_tbl->count; i++) {
		memcpy(cnic_tbl->wwpn[i], tbl->settings[i].npiv_wwpn, 8);
		memcpy(cnic_tbl->wwnn[i], tbl->settings[i].npiv_wwnn, 8);
	}

	rc = 0;
out:
	kfree(tbl);
	return rc;
}