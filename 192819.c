static int hclge_bp_setup_hw(struct hclge_dev *hdev, u8 tc)
{
	int i;

	for (i = 0; i < HCLGE_BP_GRP_NUM; i++) {
		u32 qs_bitmap = 0;
		int k, ret;

		for (k = 0; k < hdev->num_alloc_vport; k++) {
			struct hclge_vport *vport = &hdev->vport[k];
			u16 qs_id = vport->qs_offset + tc;
			u8 grp, sub_grp;

			grp = hnae3_get_field(qs_id, HCLGE_BP_GRP_ID_M,
					      HCLGE_BP_GRP_ID_S);
			sub_grp = hnae3_get_field(qs_id, HCLGE_BP_SUB_GRP_ID_M,
						  HCLGE_BP_SUB_GRP_ID_S);
			if (i == grp)
				qs_bitmap |= (1 << sub_grp);
		}

		ret = hclge_tm_qs_bp_cfg(hdev, tc, i, qs_bitmap);
		if (ret)
			return ret;
	}

	return 0;
}