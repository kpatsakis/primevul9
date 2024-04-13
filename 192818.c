static int hclge_tm_pg_dwrr_cfg(struct hclge_dev *hdev)
{
	int ret;
	u32 i;

	/* cfg pg schd */
	if (hdev->tx_sch_mode != HCLGE_FLAG_TC_BASE_SCH_MODE)
		return 0;

	/* pg to prio */
	for (i = 0; i < hdev->tm_info.num_pg; i++) {
		/* Cfg dwrr */
		ret = hclge_tm_pg_weight_cfg(hdev, i, hdev->tm_info.pg_dwrr[i]);
		if (ret)
			return ret;
	}

	return 0;
}