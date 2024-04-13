int hclge_pause_addr_cfg(struct hclge_dev *hdev, const u8 *mac_addr)
{
	struct hclge_cfg_pause_param_cmd *pause_param;
	struct hclge_desc desc;
	u16 trans_time;
	u8 trans_gap;
	int ret;

	pause_param = (struct hclge_cfg_pause_param_cmd *)desc.data;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_CFG_MAC_PARA, true);

	ret = hclge_cmd_send(&hdev->hw, &desc, 1);
	if (ret)
		return ret;

	trans_gap = pause_param->pause_trans_gap;
	trans_time = le16_to_cpu(pause_param->pause_trans_time);

	return hclge_pause_param_cfg(hdev, mac_addr, trans_gap, trans_time);
}