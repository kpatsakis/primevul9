static int hclge_pfc_setup_hw(struct hclge_dev *hdev)
{
	u8 enable_bitmap = 0;

	if (hdev->tm_info.fc_mode == HCLGE_FC_PFC)
		enable_bitmap = HCLGE_TX_MAC_PAUSE_EN_MSK |
				HCLGE_RX_MAC_PAUSE_EN_MSK;

	return hclge_pfc_pause_en_cfg(hdev, enable_bitmap,
				      hdev->tm_info.pfc_en);
}