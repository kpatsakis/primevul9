static int hclge_tm_pg_to_pri_map(struct hclge_dev *hdev)
{
	int ret;
	u32 i;

	if (hdev->tx_sch_mode != HCLGE_FLAG_TC_BASE_SCH_MODE)
		return 0;

	for (i = 0; i < hdev->tm_info.num_pg; i++) {
		/* Cfg mapping */
		ret = hclge_tm_pg_to_pri_map_cfg(
			hdev, i, hdev->tm_info.pg_info[i].tc_bit_map);
		if (ret)
			return ret;
	}

	return 0;
}