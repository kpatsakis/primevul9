int hclge_tm_schd_init(struct hclge_dev *hdev)
{
	int ret;

	/* fc_mode is HCLGE_FC_FULL on reset */
	hdev->tm_info.fc_mode = HCLGE_FC_FULL;
	hdev->fc_mode_last_time = hdev->tm_info.fc_mode;

	ret = hclge_tm_schd_info_init(hdev);
	if (ret)
		return ret;

	return hclge_tm_init_hw(hdev, true);
}