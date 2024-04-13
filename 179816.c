static int mwifiex_pcie_resume(struct device *dev)
{
	struct mwifiex_adapter *adapter;
	struct pcie_service_card *card = dev_get_drvdata(dev);


	if (!card->adapter) {
		dev_err(dev, "adapter structure is not valid\n");
		return 0;
	}

	adapter = card->adapter;

	if (!test_bit(MWIFIEX_IS_SUSPENDED, &adapter->work_flags)) {
		mwifiex_dbg(adapter, WARN,
			    "Device already resumed\n");
		return 0;
	}

	clear_bit(MWIFIEX_IS_SUSPENDED, &adapter->work_flags);

	mwifiex_cancel_hs(mwifiex_get_priv(adapter, MWIFIEX_BSS_ROLE_STA),
			  MWIFIEX_ASYNC_CMD);
	mwifiex_disable_wake(adapter);

	return 0;
}