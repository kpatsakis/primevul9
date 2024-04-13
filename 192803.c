static int hclge_tm_pri_dwrr_cfg(struct hclge_dev *hdev)
{
	int ret;

	if (hdev->tx_sch_mode == HCLGE_FLAG_TC_BASE_SCH_MODE) {
		ret = hclge_tm_pri_tc_base_dwrr_cfg(hdev);
		if (ret)
			return ret;

		if (!hnae3_dev_dcb_supported(hdev))
			return 0;

		ret = hclge_tm_ets_tc_dwrr_cfg(hdev);
		if (ret == -EOPNOTSUPP) {
			dev_warn(&hdev->pdev->dev,
				 "fw %08x does't support ets tc weight cmd\n",
				 hdev->fw_version);
			ret = 0;
		}

		return ret;
	} else {
		ret = hclge_tm_pri_vnet_base_dwrr_cfg(hdev);
		if (ret)
			return ret;
	}

	return 0;
}