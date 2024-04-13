static int hclge_tm_pri_vnet_base_dwrr_cfg(struct hclge_dev *hdev)
{
	struct hclge_vport *vport = hdev->vport;
	int ret;
	u32 i;

	for (i = 0; i < hdev->num_alloc_vport; i++) {
		ret = hclge_tm_pri_vnet_base_dwrr_pri_cfg(vport);
		if (ret)
			return ret;

		vport++;
	}

	return 0;
}