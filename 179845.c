static void mwifiex_pcie_disable_host_int_noerr(struct mwifiex_adapter *adapter)
{
	WARN_ON(mwifiex_pcie_disable_host_int(adapter));
}