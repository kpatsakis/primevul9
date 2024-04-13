int hclge_mac_pause_en_cfg(struct hclge_dev *hdev, bool tx, bool rx)
{
	struct hclge_desc desc;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_CFG_MAC_PAUSE_EN, false);

	desc.data[0] = cpu_to_le32((tx ? HCLGE_TX_MAC_PAUSE_EN_MSK : 0) |
		(rx ? HCLGE_RX_MAC_PAUSE_EN_MSK : 0));

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}