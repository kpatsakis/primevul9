static int hclge_tm_lvl34_schd_mode_cfg(struct hclge_dev *hdev)
{
	struct hclge_vport *vport = hdev->vport;
	int ret;
	u8 i, k;

	if (hdev->tx_sch_mode == HCLGE_FLAG_TC_BASE_SCH_MODE) {
		for (i = 0; i < hdev->tm_info.num_tc; i++) {
			ret = hclge_tm_pri_schd_mode_cfg(hdev, i);
			if (ret)
				return ret;

			for (k = 0; k < hdev->num_alloc_vport; k++) {
				ret = hclge_tm_qs_schd_mode_cfg(
					hdev, vport[k].qs_offset + i,
					HCLGE_SCH_MODE_DWRR);
				if (ret)
					return ret;
			}
		}
	} else {
		for (i = 0; i < hdev->num_alloc_vport; i++) {
			ret = hclge_tm_schd_mode_vnet_base_cfg(vport);
			if (ret)
				return ret;

			vport++;
		}
	}

	return 0;
}