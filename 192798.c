static int hclge_tm_shaper_cfg(struct hclge_dev *hdev)
{
	int ret;

	ret = hclge_tm_port_shaper_cfg(hdev);
	if (ret)
		return ret;

	ret = hclge_tm_pg_shaper_cfg(hdev);
	if (ret)
		return ret;

	return hclge_tm_pri_shaper_cfg(hdev);
}