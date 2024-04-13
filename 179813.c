mwifiex_check_fw_status(struct mwifiex_adapter *adapter, u32 poll_num)
{
	int ret = 0;
	u32 firmware_stat;
	struct pcie_service_card *card = adapter->card;
	const struct mwifiex_pcie_card_reg *reg = card->pcie.reg;
	u32 tries;

	/* Mask spurios interrupts */
	if (mwifiex_write_reg(adapter, PCIE_HOST_INT_STATUS_MASK,
			      HOST_INTR_MASK)) {
		mwifiex_dbg(adapter, ERROR,
			    "Write register failed\n");
		return -1;
	}

	mwifiex_dbg(adapter, INFO,
		    "Setting driver ready signature\n");
	if (mwifiex_write_reg(adapter, reg->drv_rdy,
			      FIRMWARE_READY_PCIE)) {
		mwifiex_dbg(adapter, ERROR,
			    "Failed to write driver ready signature\n");
		return -1;
	}

	/* Wait for firmware initialization event */
	for (tries = 0; tries < poll_num; tries++) {
		if (mwifiex_read_reg(adapter, reg->fw_status,
				     &firmware_stat))
			ret = -1;
		else
			ret = 0;

		mwifiex_dbg(adapter, INFO, "Try %d if FW is ready <%d,%#x>",
			    tries, ret, firmware_stat);

		if (ret)
			continue;
		if (firmware_stat == FIRMWARE_READY_PCIE) {
			ret = 0;
			break;
		} else {
			msleep(100);
			ret = -1;
		}
	}

	return ret;
}