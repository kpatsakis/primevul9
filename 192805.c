static int hclge_pause_param_setup_hw(struct hclge_dev *hdev)
{
	struct hclge_mac *mac = &hdev->hw.mac;

	return hclge_pause_param_cfg(hdev, mac->mac_addr,
				     HCLGE_DEFAULT_PAUSE_TRANS_GAP,
				     HCLGE_DEFAULT_PAUSE_TRANS_TIME);
}