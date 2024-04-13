static int hclge_tm_lvl2_schd_mode_cfg(struct hclge_dev *hdev)
{
	int ret;
	u8 i;

	/* Only being config on TC-Based scheduler mode */
	if (hdev->tx_sch_mode == HCLGE_FLAG_VNET_BASE_SCH_MODE)
		return 0;

	for (i = 0; i < hdev->tm_info.num_pg; i++) {
		ret = hclge_tm_pg_schd_mode_cfg(hdev, i);
		if (ret)
			return ret;
	}

	return 0;
}