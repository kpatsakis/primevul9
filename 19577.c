static int iwl_trans_pcie_rxq_dma_data(struct iwl_trans *trans, int queue,
				       struct iwl_trans_rxq_dma_data *data)
{
	struct iwl_trans_pcie *trans_pcie = IWL_TRANS_GET_PCIE_TRANS(trans);

	if (queue >= trans->num_rx_queues || !trans_pcie->rxq)
		return -EINVAL;

	data->fr_bd_cb = trans_pcie->rxq[queue].bd_dma;
	data->urbd_stts_wrptr = trans_pcie->rxq[queue].rb_stts_dma;
	data->ur_bd_cb = trans_pcie->rxq[queue].used_bd_dma;
	data->fr_bd_wid = 0;

	return 0;
}