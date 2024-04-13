int hclge_pfc_rx_stats_get(struct hclge_dev *hdev, u64 *stats)
{
	return hclge_pfc_stats_get(hdev, HCLGE_OPC_QUERY_PFC_RX_PKT_CNT, stats);
}