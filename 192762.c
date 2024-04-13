static int hclge_pfc_pause_en_cfg(struct hclge_dev *hdev, u8 tx_rx_bitmap,
				  u8 pfc_bitmap)
{
	struct hclge_desc desc;
	struct hclge_pfc_en_cmd *pfc = (struct hclge_pfc_en_cmd *)desc.data;

	hclge_cmd_setup_basic_desc(&desc, HCLGE_OPC_CFG_PFC_PAUSE_EN, false);

	pfc->tx_rx_en_bitmap = tx_rx_bitmap;
	pfc->pri_en_bitmap = pfc_bitmap;

	return hclge_cmd_send(&hdev->hw, &desc, 1);
}