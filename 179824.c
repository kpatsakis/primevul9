static int mwifiex_register_dev(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;

	/* save adapter pointer in card */
	card->adapter = adapter;

	if (mwifiex_pcie_request_irq(adapter))
		return -1;

	adapter->tx_buf_size = card->pcie.tx_buf_size;
	adapter->mem_type_mapping_tbl = card->pcie.mem_type_mapping_tbl;
	adapter->num_mem_types = card->pcie.num_mem_types;
	adapter->ext_scan = card->pcie.can_ext_scan;
	mwifiex_pcie_get_fw_name(adapter);

	return 0;
}