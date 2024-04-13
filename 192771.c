static int hclge_tm_pri_shaper_cfg(struct hclge_dev *hdev)
{
	int ret;

	if (hdev->tx_sch_mode == HCLGE_FLAG_TC_BASE_SCH_MODE) {
		ret = hclge_tm_pri_tc_base_shaper_cfg(hdev);
		if (ret)
			return ret;
	} else {
		ret = hclge_tm_pri_vnet_base_shaper_cfg(hdev);
		if (ret)
			return ret;
	}

	return 0;
}