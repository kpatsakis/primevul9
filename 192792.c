static int hclge_tm_pg_shaper_cfg(struct hclge_dev *hdev)
{
	u8 ir_u, ir_b, ir_s;
	u32 shaper_para;
	int ret;
	u32 i;

	/* Cfg pg schd */
	if (hdev->tx_sch_mode != HCLGE_FLAG_TC_BASE_SCH_MODE)
		return 0;

	/* Pg to pri */
	for (i = 0; i < hdev->tm_info.num_pg; i++) {
		/* Calc shaper para */
		ret = hclge_shaper_para_calc(
					hdev->tm_info.pg_info[i].bw_limit,
					HCLGE_SHAPER_LVL_PG,
					&ir_b, &ir_u, &ir_s);
		if (ret)
			return ret;

		shaper_para = hclge_tm_get_shapping_para(0, 0, 0,
							 HCLGE_SHAPER_BS_U_DEF,
							 HCLGE_SHAPER_BS_S_DEF);
		ret = hclge_tm_pg_shapping_cfg(hdev,
					       HCLGE_TM_SHAP_C_BUCKET, i,
					       shaper_para);
		if (ret)
			return ret;

		shaper_para = hclge_tm_get_shapping_para(ir_b, ir_u, ir_s,
							 HCLGE_SHAPER_BS_U_DEF,
							 HCLGE_SHAPER_BS_S_DEF);
		ret = hclge_tm_pg_shapping_cfg(hdev,
					       HCLGE_TM_SHAP_P_BUCKET, i,
					       shaper_para);
		if (ret)
			return ret;
	}

	return 0;
}