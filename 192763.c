static int hclge_pause_param_cfg(struct hclge_dev *hdev, const u8 *addr,
				 u8 pause_trans_gap, u16 pause_trans_time)
{
	struct hclge_cfg_pause_param_cmd *pause_param;
	struct hclge_desc desc;

	pause_param = (struct hclge_cfg_pause_param_cmd *)desc.data;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_CFG_MAC_PARA, false);

	ether_addr_copy(pause_param->mac_addr, addr);
	ether_addr_copy(pause_param->mac_addr_extra, addr);
	pause_param->pause_trans_gap = pause_trans_gap;
	pause_param->pause_trans_time = cpu_to_le16(pause_trans_time);

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}