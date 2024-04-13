int hclge_tm_schd_setup_hw(struct hclge_dev *hdev)
{
	int ret;

	/* Cfg tm mapping  */
	ret = hclge_tm_map_cfg(hdev);
	if (ret)
		return ret;

	/* Cfg tm shaper */
	ret = hclge_tm_shaper_cfg(hdev);
	if (ret)
		return ret;

	/* Cfg dwrr */
	ret = hclge_tm_dwrr_cfg(hdev);
	if (ret)
		return ret;

	/* Cfg schd mode for each level schd */
	return hclge_tm_schd_mode_hw(hdev);
}