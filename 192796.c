static int hclge_tm_qs_to_pri_map_cfg(struct hclge_dev *hdev,
				      u16 qs_id, u8 pri)
{
	struct hclge_qs_to_pri_link_cmd *map;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_QS_TO_PRI_LINK, false);

	map = (struct hclge_qs_to_pri_link_cmd *)desc.data;

	map->qs_id = cpu_to_le16(qs_id);
	map->priority = pri;
	map->link_vld = HCLGE_TM_QS_PRI_LINK_VLD_MSK;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}