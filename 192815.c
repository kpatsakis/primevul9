static int hclge_up_to_tc_map(struct hclge_dev *hdev)
{
	struct hclge_desc desc;
	u8 *pri = (u8 *)desc.data;
	u8 pri_id;
	int ret;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_PRI_TO_TC_MAPPING, false);

	for (pri_id = 0; pri_id < HNAE3_MAX_USER_PRIO; pri_id++) {
		ret = hclge_fill_pri_array(hdev, pri, pri_id);
		if (ret)
			return ret;
	}

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}