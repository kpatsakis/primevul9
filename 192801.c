static void hclge_tm_vport_info_update(struct hclge_dev *hdev)
{
	struct hclge_vport *vport = hdev->vport;
	u32 i;

	for (i = 0; i < hdev->num_alloc_vport; i++) {
		hclge_tm_vport_tc_info_update(vport);

		vport++;
	}
}