static int mwifiex_extract_wifi_fw(struct mwifiex_adapter *adapter,
				   const void *firmware, u32 firmware_len) {
	const struct mwifiex_fw_data *fwdata;
	u32 offset = 0, data_len, dnld_cmd;
	int ret = 0;
	bool cmd7_before = false, first_cmd = false;

	while (1) {
		/* Check for integer and buffer overflow */
		if (offset + sizeof(fwdata->header) < sizeof(fwdata->header) ||
		    offset + sizeof(fwdata->header) >= firmware_len) {
			mwifiex_dbg(adapter, ERROR,
				    "extract wifi-only fw failure!\n");
			ret = -1;
			goto done;
		}

		fwdata = firmware + offset;
		dnld_cmd = le32_to_cpu(fwdata->header.dnld_cmd);
		data_len = le32_to_cpu(fwdata->header.data_length);

		/* Skip past header */
		offset += sizeof(fwdata->header);

		switch (dnld_cmd) {
		case MWIFIEX_FW_DNLD_CMD_1:
			if (offset + data_len < data_len) {
				mwifiex_dbg(adapter, ERROR, "bad FW parse\n");
				ret = -1;
				goto done;
			}

			/* Image start with cmd1, already wifi-only firmware */
			if (!first_cmd) {
				mwifiex_dbg(adapter, MSG,
					    "input wifi-only firmware\n");
				return 0;
			}

			if (!cmd7_before) {
				mwifiex_dbg(adapter, ERROR,
					    "no cmd7 before cmd1!\n");
				ret = -1;
				goto done;
			}
			offset += data_len;
			break;
		case MWIFIEX_FW_DNLD_CMD_5:
			first_cmd = true;
			/* Check for integer overflow */
			if (offset + data_len < data_len) {
				mwifiex_dbg(adapter, ERROR, "bad FW parse\n");
				ret = -1;
				goto done;
			}
			offset += data_len;
			break;
		case MWIFIEX_FW_DNLD_CMD_6:
			first_cmd = true;
			/* Check for integer overflow */
			if (offset + data_len < data_len) {
				mwifiex_dbg(adapter, ERROR, "bad FW parse\n");
				ret = -1;
				goto done;
			}
			offset += data_len;
			if (offset >= firmware_len) {
				mwifiex_dbg(adapter, ERROR,
					    "extract wifi-only fw failure!\n");
				ret = -1;
			} else {
				ret = offset;
			}
			goto done;
		case MWIFIEX_FW_DNLD_CMD_7:
			first_cmd = true;
			cmd7_before = true;
			break;
		default:
			mwifiex_dbg(adapter, ERROR, "unknown dnld_cmd %d\n",
				    dnld_cmd);
			ret = -1;
			goto done;
		}
	}

done:
	return ret;
}