mwifiex_pcie_rdwr_firmware(struct mwifiex_adapter *adapter, u8 doneflag)
{
	int ret, tries;
	u8 ctrl_data;
	u32 fw_status;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;

	if (mwifiex_read_reg(adapter, reg->fw_status, &fw_status))
		return RDWR_STATUS_FAILURE;

	ret = mwifiex_write_reg(adapter, reg->fw_dump_ctrl,
				reg->fw_dump_host_ready);
	if (ret) {
		mwifiex_dbg(adapter, ERROR,
			    "PCIE write err\n");
		return RDWR_STATUS_FAILURE;
	}

	for (tries = 0; tries < MAX_POLL_TRIES; tries++) {
		mwifiex_read_reg_byte(adapter, reg->fw_dump_ctrl, &ctrl_data);
		if (ctrl_data == FW_DUMP_DONE)
			return RDWR_STATUS_SUCCESS;
		if (doneflag && ctrl_data == doneflag)
			return RDWR_STATUS_DONE;
		if (ctrl_data != reg->fw_dump_host_ready) {
			mwifiex_dbg(adapter, WARN,
				    "The ctrl reg was changed, re-try again!\n");
			ret = mwifiex_write_reg(adapter, reg->fw_dump_ctrl,
						reg->fw_dump_host_ready);
			if (ret) {
				mwifiex_dbg(adapter, ERROR,
					    "PCIE write err\n");
				return RDWR_STATUS_FAILURE;
			}
		}
		usleep_range(100, 200);
	}

	mwifiex_dbg(adapter, ERROR, "Fail to pull ctrl_data\n");
	return RDWR_STATUS_FAILURE;
}