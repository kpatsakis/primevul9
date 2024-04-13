static int hclge_tm_map_cfg(struct hclge_dev *hdev)
{
	int ret;

	ret = hclge_up_to_tc_map(hdev);
	if (ret)
		return ret;

	ret = hclge_tm_pg_to_pri_map(hdev);
	if (ret)
		return ret;

	return hclge_tm_pri_q_qs_cfg(hdev);
}