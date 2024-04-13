static int hclge_pfc_stats_get(struct hclge_dev *hdev,
			       enum hclge_opcode_type opcode, u64 *stats)
{
	struct hclge_desc desc[HCLGE_TM_PFC_PKT_GET_CMD_NUM];
	int ret, i, j;

	if (!(opcode == HCLGE_OPC_QUERY_PFC_RX_PKT_CNT ||
	      opcode == HCLGE_OPC_QUERY_PFC_TX_PKT_CNT))
		return -EINVAL;

	for (i = 0; i < HCLGE_TM_PFC_PKT_GET_CMD_NUM - 1; i++) {
		hclge_cmd_setup_basic_desc(&desc[i], opcode, true);
		desc[i].flag |= cpu_to_le16(HCLGE_CMD_FLAG_NEXT);
	}

	hclge_cmd_setup_basic_desc(&desc[i], opcode, true);

	ret = hclge_cmd_send(&hdev->hw, desc, HCLGE_TM_PFC_PKT_GET_CMD_NUM);
	if (ret)
		return ret;

	for (i = 0; i < HCLGE_TM_PFC_PKT_GET_CMD_NUM; i++) {
		struct hclge_pfc_stats_cmd *pfc_stats =
				(struct hclge_pfc_stats_cmd *)desc[i].data;

		for (j = 0; j < HCLGE_TM_PFC_NUM_GET_PER_CMD; j++) {
			u32 index = i * HCLGE_TM_PFC_PKT_GET_CMD_NUM + j;

			if (index < HCLGE_MAX_TC_NUM)
				stats[index] =
					le64_to_cpu(pfc_stats->pkt_num[j]);
		}
	}
	return 0;
}