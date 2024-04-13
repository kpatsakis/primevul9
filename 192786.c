static int hclge_tm_ets_tc_dwrr_cfg(struct hclge_dev *hdev)
{
#define DEFAULT_TC_WEIGHT	1
#define DEFAULT_TC_OFFSET	14

	struct hclge_ets_tc_weight_cmd *ets_weight;
	struct hclge_desc desc;
	unsigned int i;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_ETS_TC_WEIGHT, false);
	ets_weight = (struct hclge_ets_tc_weight_cmd *)desc.data;

	for (i = 0; i < HNAE3_MAX_TC; i++) {
		struct hclge_pg_info *pg_info;

		ets_weight->tc_weight[i] = DEFAULT_TC_WEIGHT;

		if (!(hdev->hw_tc_map & BIT(i)))
			continue;

		pg_info =
			&hdev->tm_info.pg_info[hdev->tm_info.tc_info[i].pgid];
		ets_weight->tc_weight[i] = pg_info->tc_dwrr[i];
	}

	ets_weight->weight_offset = DEFAULT_TC_OFFSET;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}