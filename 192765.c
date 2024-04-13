void hclge_tm_prio_tc_info_update(struct hclge_dev *hdev, u8 *prio_tc)
{
	struct hclge_vport *vport = hdev->vport;
	struct hnae3_knic_private_info *kinfo;
	u32 i, k;

	for (i = 0; i < HNAE3_MAX_USER_PRIO; i++) {
		hdev->tm_info.prio_tc[i] = prio_tc[i];

		for (k = 0;  k < hdev->num_alloc_vport; k++) {
			kinfo = &vport[k].nic.kinfo;
			kinfo->prio_tc[i] = prio_tc[i];
		}
	}
}