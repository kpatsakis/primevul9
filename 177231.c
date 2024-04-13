static int _extend_sdma_tx_descs(struct hfi1_devdata *dd, struct sdma_txreq *tx)
{
	int i;

	/* Handle last descriptor */
	if (unlikely((tx->num_desc == (MAX_DESC - 1)))) {
		/* if tlen is 0, it is for padding, release last descriptor */
		if (!tx->tlen) {
			tx->desc_limit = MAX_DESC;
		} else if (!tx->coalesce_buf) {
			/* allocate coalesce buffer with space for padding */
			tx->coalesce_buf = kmalloc(tx->tlen + sizeof(u32),
						   GFP_ATOMIC);
			if (!tx->coalesce_buf)
				goto enomem;
			tx->coalesce_idx = 0;
		}
		return 0;
	}

	if (unlikely(tx->num_desc == MAX_DESC))
		goto enomem;

	tx->descp = kmalloc_array(
			MAX_DESC,
			sizeof(struct sdma_desc),
			GFP_ATOMIC);
	if (!tx->descp)
		goto enomem;

	/* reserve last descriptor for coalescing */
	tx->desc_limit = MAX_DESC - 1;
	/* copy ones already built */
	for (i = 0; i < tx->num_desc; i++)
		tx->descp[i] = tx->descs[i];
	return 0;
enomem:
	__sdma_txclean(dd, tx);
	return -ENOMEM;
}