static int hclge_tm_schd_info_init(struct hclge_dev *hdev)
{
	if ((hdev->tx_sch_mode != HCLGE_FLAG_TC_BASE_SCH_MODE) &&
	    (hdev->tm_info.num_pg != 1))
		return -EINVAL;

	hclge_tm_pg_info_init(hdev);

	hclge_tm_tc_info_init(hdev);

	hclge_tm_vport_info_update(hdev);

	hclge_pfc_info_init(hdev);

	return 0;
}