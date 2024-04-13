static int hclge_tm_schd_mode_hw(struct hclge_dev *hdev)
{
	int ret;

	ret = hclge_tm_lvl2_schd_mode_cfg(hdev);
	if (ret)
		return ret;

	return hclge_tm_lvl34_schd_mode_cfg(hdev);
}