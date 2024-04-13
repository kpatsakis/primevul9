static int hclge_tm_pg_weight_cfg(struct hclge_dev *hdev, u8 pg_id,
				  u8 dwrr)
{
	struct hclge_pg_weight_cmd *weight;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_PG_WEIGHT, false);

	weight = (struct hclge_pg_weight_cmd *)desc.data;

	weight->pg_id = pg_id;
	weight->dwrr = dwrr;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}