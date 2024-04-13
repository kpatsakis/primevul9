static int mwifiex_pcie_suspend(struct device *dev)
{
	struct mwifiex_adapter *adapter;
	struct pcie_service_card *card = dev_get_drvdata(dev);


	/* Might still be loading firmware */
	wait_for_completion(&card->fw_done);

	adapter = card->adapter;
	if (!adapter) {
		dev_err(dev, "adapter is not valid\n");
		return 0;
	}

	mwifiex_enable_wake(adapter);

	/* Enable the Host Sleep */
	if (!mwifiex_enable_hs(adapter)) {
		mwifiex_dbg(adapter, ERROR,
			    "cmd: failed to suspend\n");
		clear_bit(MWIFIEX_IS_HS_ENABLING, &adapter->work_flags);
		mwifiex_disable_wake(adapter);
		return -EFAULT;
	}

	flush_workqueue(adapter->workqueue);

	/* Indicate device suspended */
	set_bit(MWIFIEX_IS_SUSPENDED, &adapter->work_flags);
	clear_bit(MWIFIEX_IS_HS_ENABLING, &adapter->work_flags);

	return 0;
}