static int hclge_tm_q_to_qs_map_cfg(struct hclge_dev *hdev,
				    u16 q_id, u16 qs_id)
{
	struct hclge_nq_to_qs_link_cmd *map;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_NQ_TO_QS_LINK, false);

	map = (struct hclge_nq_to_qs_link_cmd *)desc.data;

	map->nq_id = cpu_to_le16(q_id);
	map->qset_id = cpu_to_le16(qs_id | HCLGE_TM_Q_QS_LINK_VLD_MSK);

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}