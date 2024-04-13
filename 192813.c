static int hclge_tm_pri_weight_cfg(struct hclge_dev *hdev, u8 pri_id,
				   u8 dwrr)
{
	struct hclge_priority_weight_cmd *weight;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_PRI_WEIGHT, false);

	weight = (struct hclge_priority_weight_cmd *)desc.data;

	weight->pri_id = pri_id;
	weight->dwrr = dwrr;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}