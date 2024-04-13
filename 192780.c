static int hclge_tm_pg_shapping_cfg(struct hclge_dev *hdev,
				    enum hclge_shap_bucket bucket, u8 pg_id,
				    u32 shapping_para)
{
	struct hclge_pg_shapping_cmd *shap_cfg_cmd;
	enum hclge_opcode_type opcode;
	struct hclge_desc desc;

	opcode = bucket ? HCLGE_OPC_TM_PG_P_SHAPPING :
		 HCLGE_OPC_TM_PG_C_SHAPPING;
	hclge_cmd_setup_basic_desc(&desc, opcode, false);

	shap_cfg_cmd = (struct hclge_pg_shapping_cmd *)desc.data;

	shap_cfg_cmd->pg_id = pg_id;

	shap_cfg_cmd->pg_shapping_para = cpu_to_le32(shapping_para);

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}