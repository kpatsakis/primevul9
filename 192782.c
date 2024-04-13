static void hclge_tm_tc_info_init(struct hclge_dev *hdev)
{
	u8 i;

	for (i = 0; i < hdev->tm_info.num_tc; i++) {
		hdev->tm_info.tc_info[i].tc_id = i;
		hdev->tm_info.tc_info[i].tc_sch_mode = HCLGE_SCH_MODE_DWRR;
		hdev->tm_info.tc_info[i].pgid = 0;
		hdev->tm_info.tc_info[i].bw_limit =
			hdev->tm_info.pg_info[0].bw_limit;
	}

	for (i = 0; i < HNAE3_MAX_USER_PRIO; i++)
		hdev->tm_info.prio_tc[i] =
			(i >= hdev->tm_info.num_tc) ? 0 : i;

	/* DCB is enabled if we have more than 1 TC or pfc_en is
	 * non-zero.
	 */
	if (hdev->tm_info.num_tc > 1 || hdev->tm_info.pfc_en)
		hdev->flag |= HCLGE_FLAG_DCB_ENABLE;
	else
		hdev->flag &= ~HCLGE_FLAG_DCB_ENABLE;
}