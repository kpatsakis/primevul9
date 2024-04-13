static int hclge_tm_qs_weight_cfg(struct hclge_dev *hdev, u16 qs_id,
				  u8 dwrr)
{
	struct hclge_qs_weight_cmd *weight;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_QS_WEIGHT, false);

	weight = (struct hclge_qs_weight_cmd *)desc.data;

	weight->qs_id = cpu_to_le16(qs_id);
	weight->dwrr = dwrr;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}