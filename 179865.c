static int mwifiex_clean_pcie_ring_buf(struct mwifiex_adapter *adapter)
{
	struct pcie_service_card *card = adapter->card;

	if (!mwifiex_pcie_txbd_empty(card, card->txbd_rdptr)) {
		card->txbd_flush = 1;
		/* write pointer already set at last send
		 * send dnld-rdy intr again, wait for completion.
		 */
		if (mwifiex_write_reg(adapter, PCIE_CPU_INT_EVENT,
				      CPU_INTR_DNLD_RDY)) {
			mwifiex_dbg(adapter, ERROR,
				    "failed to assert dnld-rdy interrupt.\n");
			return -1;
		}
	}
	return 0;
}