static int hclge_tm_pri_q_qs_cfg(struct hclge_dev *hdev)
{
	struct hclge_vport *vport = hdev->vport;
	int ret;
	u32 i, k;

	if (hdev->tx_sch_mode == HCLGE_FLAG_TC_BASE_SCH_MODE) {
		/* Cfg qs -> pri mapping, one by one mapping */
		for (k = 0; k < hdev->num_alloc_vport; k++) {
			struct hnae3_knic_private_info *kinfo =
				&vport[k].nic.kinfo;

			for (i = 0; i < kinfo->num_tc; i++) {
				ret = hclge_tm_qs_to_pri_map_cfg(
					hdev, vport[k].qs_offset + i, i);
				if (ret)
					return ret;
			}
		}
	} else if (hdev->tx_sch_mode == HCLGE_FLAG_VNET_BASE_SCH_MODE) {
		/* Cfg qs -> pri mapping,  qs = tc, pri = vf, 8 qs -> 1 pri */
		for (k = 0; k < hdev->num_alloc_vport; k++)
			for (i = 0; i < HNAE3_MAX_TC; i++) {
				ret = hclge_tm_qs_to_pri_map_cfg(
					hdev, vport[k].qs_offset + i, k);
				if (ret)
					return ret;
			}
	} else {
		return -EINVAL;
	}

	/* Cfg q -> qs mapping */
	for (i = 0; i < hdev->num_alloc_vport; i++) {
		ret = hclge_vport_q_to_qs_map(hdev, vport);
		if (ret)
			return ret;

		vport++;
	}

	return 0;
}