static int hclge_tm_pri_tc_base_dwrr_cfg(struct hclge_dev *hdev)
{
	struct hclge_vport *vport = hdev->vport;
	struct hclge_pg_info *pg_info;
	u8 dwrr;
	int ret;
	u32 i, k;

	for (i = 0; i < hdev->tm_info.num_tc; i++) {
		pg_info =
			&hdev->tm_info.pg_info[hdev->tm_info.tc_info[i].pgid];
		dwrr = pg_info->tc_dwrr[i];

		ret = hclge_tm_pri_weight_cfg(hdev, i, dwrr);
		if (ret)
			return ret;

		for (k = 0; k < hdev->num_alloc_vport; k++) {
			ret = hclge_tm_qs_weight_cfg(
				hdev, vport[k].qs_offset + i,
				vport[k].dwrr);
			if (ret)
				return ret;
		}
	}

	return 0;
}