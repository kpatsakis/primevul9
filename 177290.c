int _pad_sdma_tx_descs(struct hfi1_devdata *dd, struct sdma_txreq *tx)
{
	int rval = 0;

	tx->num_desc++;
	if ((unlikely(tx->num_desc == tx->desc_limit))) {
		rval = _extend_sdma_tx_descs(dd, tx);
		if (rval) {
			__sdma_txclean(dd, tx);
			return rval;
		}
	}
	/* finish the one just added */
	make_tx_sdma_desc(
		tx,
		SDMA_MAP_NONE,
		dd->sdma_pad_phys,
		sizeof(u32) - (tx->packet_len & (sizeof(u32) - 1)));
	_sdma_close_tx(dd, tx);
	return rval;
}