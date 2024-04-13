mwifiex_pcie_reg_dump(struct mwifiex_adapter *adapter, char *drv_buf)
{
	char *p = drv_buf;
	char buf[256], *ptr;
	int i;
	u32 value;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	int pcie_scratch_reg[] = {PCIE_SCRATCH_12_REG,
				  PCIE_SCRATCH_14_REG,
				  PCIE_SCRATCH_15_REG};

	if (!p)
		return 0;

	mwifiex_dbg(adapter, MSG, "PCIE register dump start\n");

	if (mwifiex_read_reg(adapter, reg->fw_status, &value)) {
		mwifiex_dbg(adapter, ERROR, "failed to read firmware status");
		return 0;
	}

	ptr = buf;
	mwifiex_dbg(adapter, MSG, "pcie scratch register:");
	for (i = 0; i < ARRAY_SIZE(pcie_scratch_reg); i++) {
		mwifiex_read_reg(adapter, pcie_scratch_reg[i], &value);
		ptr += sprintf(ptr, "reg:0x%x, value=0x%x\n",
			       pcie_scratch_reg[i], value);
	}

	mwifiex_dbg(adapter, MSG, "%s\n", buf);
	p += sprintf(p, "%s\n", buf);

	mwifiex_dbg(adapter, MSG, "PCIE register dump end\n");

	return p - drv_buf;
}