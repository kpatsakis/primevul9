static int hclge_tm_bp_setup(struct hclge_dev *hdev)
{
	int ret = 0;
	int i;

	for (i = 0; i < hdev->tm_info.num_tc; i++) {
		ret = hclge_bp_setup_hw(hdev, i);
		if (ret)
			return ret;
	}

	return ret;
}