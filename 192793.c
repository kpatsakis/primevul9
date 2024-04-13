static int hclge_tm_qs_bp_cfg(struct hclge_dev *hdev, u8 tc, u8 grp_id,
			      u32 bit_map)
{
	struct hclge_bp_to_qs_map_cmd *bp_to_qs_map_cmd;
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_BP_TO_QSET_MAPPING,
				   false);

	bp_to_qs_map_cmd = (struct hclge_bp_to_qs_map_cmd *)desc.data;

	bp_to_qs_map_cmd->tc_id = tc;
	bp_to_qs_map_cmd->qs_group_id = grp_id;
	bp_to_qs_map_cmd->qs_bit_map = cpu_to_le32(bit_map);

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}