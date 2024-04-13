static void mwifiex_pcie_dev_wakeup_delay(struct mwifiex_adapter *adapter)
{
	int i = 0;

	while (mwifiex_pcie_ok_to_access_hw(adapter)) {
		i++;
		usleep_range(10, 20);
		/* 50ms max wait */
		if (i == 5000)
			break;
	}

	return;
}