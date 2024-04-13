static int hclge_tm_port_shaper_cfg(struct hclge_dev *hdev)
{
	struct hclge_port_shapping_cmd *shap_cfg_cmd;
	struct hclge_desc desc;
	u32 shapping_para = 0;
	u8 ir_u, ir_b, ir_s;
	int ret;

	ret = hclge_shaper_para_calc(hdev->hw.mac.speed,
				     HCLGE_SHAPER_LVL_PORT,
				     &ir_b, &ir_u, &ir_s);
	if (ret)
		return ret;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_TM_PORT_SHAPPING, false);
	shap_cfg_cmd = (struct hclge_port_shapping_cmd *)desc.data;

	shapping_para = hclge_tm_get_shapping_para(ir_b, ir_u, ir_s,
						   HCLGE_SHAPER_BS_U_DEF,
						   HCLGE_SHAPER_BS_S_DEF);

	shap_cfg_cmd->port_shapping_para = cpu_to_le32(shapping_para);

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}