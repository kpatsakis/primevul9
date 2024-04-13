int hclge_pfc_tx_stats_get(struct hclge_dev *hdev, u64 *stats)
{
	return hclge_pfc_stats_get(hdev, HCLGE_OPC_QUERY_PFC_TX_PKT_CNT, stats);
}