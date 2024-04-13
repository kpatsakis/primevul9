static void mwifiex_pcie_device_dump_work(struct mwifiex_adapter *adapter)
{
	adapter->devdump_data = vzalloc(MWIFIEX_FW_DUMP_SIZE);
	if (!adapter->devdump_data) {
		mwifiex_dbg(adapter, ERROR,
			    "vzalloc devdump data failure!\n");
		return;
	}

	mwifiex_drv_info_dump(adapter);
	mwifiex_pcie_fw_dump(adapter);
	mwifiex_prepare_fw_dump_info(adapter);
	mwifiex_upload_device_dump(adapter);
}