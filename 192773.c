static int hclge_tm_pg_schd_mode_cfg(struct hclge_dev *hdev, u8 pg_id)
{
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_PG_SCH_MODE_CFG, false);

	if (hdev->tm_info.pg_info[pg_id].pg_sch_mode == HCLGE_SCH_MODE_DWRR)
		desc.data[1] = cpu_to_le32(HCLGE_TM_TX_SCHD_DWRR_MSK);
	else
		desc.data[1] = 0;

	desc.data[0] = cpu_to_le32(pg_id);

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}