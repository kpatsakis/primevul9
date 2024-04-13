static int hclge_mac_pause_setup_hw(struct hclge_dev *hdev)
{
	bool tx_en, rx_en;

	switch (hdev->tm_info.fc_mode) {
	case HCLGE_FC_NONE:
		tx_en = false;
		rx_en = false;
		break;
	case HCLGE_FC_RX_PAUSE:
		tx_en = false;
		rx_en = true;
		break;
	case HCLGE_FC_TX_PAUSE:
		tx_en = true;
		rx_en = false;
		break;
	case HCLGE_FC_FULL:
		tx_en = true;
		rx_en = true;
		break;
	case HCLGE_FC_PFC:
		tx_en = false;
		rx_en = false;
		break;
	default:
		tx_en = true;
		rx_en = true;
	}

	return hclge_mac_pause_en_cfg(hdev, tx_en, rx_en);
}