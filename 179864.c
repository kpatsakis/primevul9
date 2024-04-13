static int mwifiex_pcie_disable_host_int(struct mwifiex_adapter *adapter)
{
	if (mwifiex_pcie_ok_to_access_hw(adapter)) {
		if (mwifiex_write_reg(adapter, PCIE_HOST_INT_MASK,
				      0x00000000)) {
			mwifiex_dbg(adapter, ERROR,
				    "Disable host interrupt failed\n");
			return -1;
		}
	}

	atomic_set(&adapter->tx_hw_pending, 0);
	return 0;
}