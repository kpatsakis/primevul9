static int hclge_tm_pri_vnet_base_shaper_qs_cfg(struct hclge_vport *vport)
{
	struct hnae3_knic_private_info *kinfo = &vport->nic.kinfo;
	struct hclge_dev *hdev = vport->back;
	u8 ir_u, ir_b, ir_s;
	u32 i;
	int ret;

	for (i = 0; i < kinfo->num_tc; i++) {
		ret = hclge_shaper_para_calc(
					hdev->tm_info.tc_info[i].bw_limit,
					HCLGE_SHAPER_LVL_QSET,
					&ir_b, &ir_u, &ir_s);
		if (ret)
			return ret;
	}

	return 0;
}