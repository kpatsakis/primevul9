static void hclge_tm_vport_tc_info_update(struct hclge_vport *vport)
{
	struct hnae3_knic_private_info *kinfo = &vport->nic.kinfo;
	struct hclge_dev *hdev = vport->back;
	u16 max_rss_size;
	u8 i;

	/* TC configuration is shared by PF/VF in one port, only allow
	 * one tc for VF for simplicity. VF's vport_id is non zero.
	 */
	kinfo->num_tc = vport->vport_id ? 1 :
			min_t(u16, vport->alloc_tqps, hdev->tm_info.num_tc);
	vport->qs_offset = (vport->vport_id ? hdev->tm_info.num_tc : 0) +
				(vport->vport_id ? (vport->vport_id - 1) : 0);

	max_rss_size = min_t(u16, hdev->rss_size_max,
			     vport->alloc_tqps / kinfo->num_tc);

	/* Set to user value, no larger than max_rss_size. */
	if (kinfo->req_rss_size != kinfo->rss_size && kinfo->req_rss_size &&
	    kinfo->req_rss_size <= max_rss_size) {
		dev_info(&hdev->pdev->dev, "rss changes from %d to %d\n",
			 kinfo->rss_size, kinfo->req_rss_size);
		kinfo->rss_size = kinfo->req_rss_size;
	} else if (kinfo->rss_size > max_rss_size ||
		   (!kinfo->req_rss_size && kinfo->rss_size < max_rss_size)) {
		/* Set to the maximum specification value (max_rss_size). */
		dev_info(&hdev->pdev->dev, "rss changes from %d to %d\n",
			 kinfo->rss_size, max_rss_size);
		kinfo->rss_size = max_rss_size;
	}

	kinfo->num_tqps = kinfo->num_tc * kinfo->rss_size;
	vport->dwrr = 100;  /* 100 percent as init */
	vport->alloc_rss_size = kinfo->rss_size;
	vport->bw_limit = hdev->tm_info.pg_info[0].bw_limit;

	for (i = 0; i < HNAE3_MAX_TC; i++) {
		if (hdev->hw_tc_map & BIT(i) && i < kinfo->num_tc) {
			kinfo->tc_info[i].enable = true;
			kinfo->tc_info[i].tqp_offset = i * kinfo->rss_size;
			kinfo->tc_info[i].tqp_count = kinfo->rss_size;
			kinfo->tc_info[i].tc = i;
		} else {
			/* Set to default queue if TC is disable */
			kinfo->tc_info[i].enable = false;
			kinfo->tc_info[i].tqp_offset = 0;
			kinfo->tc_info[i].tqp_count = 1;
			kinfo->tc_info[i].tc = 0;
		}
	}

	memcpy(kinfo->prio_tc, hdev->tm_info.prio_tc,
	       FIELD_SIZEOF(struct hnae3_knic_private_info, prio_tc));
}