static int mwifiex_pcie_enable_host_int(struct mwifiex_adapter *adapter)
{
	if (mwifiex_pcie_ok_to_access_hw(adapter)) {
		/* Simply write the mask to the register */
		if (mwifiex_write_reg(adapter, PCIE_HOST_INT_MASK,
				      HOST_INTR_MASK)) {
			mwifiex_dbg(adapter, ERROR,
				    "Enable host interrupt failed\n");
			return -1;
		}
	}

	return 0;
}