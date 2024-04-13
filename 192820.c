static int hclge_vport_q_to_qs_map(struct hclge_dev *hdev,
				   struct hclge_vport *vport)
{
	struct hnae3_knic_private_info *kinfo = &vport->nic.kinfo;
	struct hnae3_queue **tqp = kinfo->tqp;
	struct hnae3_tc_info *v_tc_info;
	u32 i, j;
	int ret;

	for (i = 0; i < kinfo->num_tc; i++) {
		v_tc_info = &kinfo->tc_info[i];
		for (j = 0; j < v_tc_info->tqp_count; j++) {
			struct hnae3_queue *q = tqp[v_tc_info->tqp_offset + j];

			ret = hclge_tm_q_to_qs_map_cfg(hdev,
						       hclge_get_queue_id(q),
						       vport->qs_offset + i);
			if (ret)
				return ret;
		}
	}

	return 0;
}