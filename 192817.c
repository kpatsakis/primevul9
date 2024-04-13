static int hclge_tm_pri_vnet_base_dwrr_pri_cfg(struct hclge_vport *vport)
{
	struct hnae3_knic_private_info *kinfo = &vport->nic.kinfo;
	struct hclge_dev *hdev = vport->back;
	int ret;
	u8 i;

	/* Vf dwrr */
	ret = hclge_tm_pri_weight_cfg(hdev, vport->vport_id, vport->dwrr);
	if (ret)
		return ret;

	/* Qset dwrr */
	for (i = 0; i < kinfo->num_tc; i++) {
		ret = hclge_tm_qs_weight_cfg(
			hdev, vport->qs_offset + i,
			hdev->tm_info.pg_info[0].tc_dwrr[i]);
		if (ret)
			return ret;
	}

	return 0;
}