static int hclge_tm_pri_vnet_base_shaper_pri_cfg(struct hclge_vport *vport)
{
	struct hclge_dev *hdev = vport->back;
	u8 ir_u, ir_b, ir_s;
	u32 shaper_para;
	int ret;

	ret = hclge_shaper_para_calc(vport->bw_limit, HCLGE_SHAPER_LVL_VF,
				     &ir_b, &ir_u, &ir_s);
	if (ret)
		return ret;

	shaper_para = hclge_tm_get_shapping_para(0, 0, 0,
						 HCLGE_SHAPER_BS_U_DEF,
						 HCLGE_SHAPER_BS_S_DEF);
	ret = hclge_tm_pri_shapping_cfg(hdev, HCLGE_TM_SHAP_C_BUCKET,
					vport->vport_id, shaper_para);
	if (ret)
		return ret;

	shaper_para = hclge_tm_get_shapping_para(ir_b, ir_u, ir_s,
						 HCLGE_SHAPER_BS_U_DEF,
						 HCLGE_SHAPER_BS_S_DEF);
	ret = hclge_tm_pri_shapping_cfg(hdev, HCLGE_TM_SHAP_P_BUCKET,
					vport->vport_id, shaper_para);
	if (ret)
		return ret;

	return 0;
}