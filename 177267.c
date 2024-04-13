void __sdma_txclean(
	struct hfi1_devdata *dd,
	struct sdma_txreq *tx)
{
	u16 i;

	if (tx->num_desc) {
		u8 skip = 0, mode = ahg_mode(tx);

		/* unmap first */
		sdma_unmap_desc(dd, &tx->descp[0]);
		/* determine number of AHG descriptors to skip */
		if (mode > SDMA_AHG_APPLY_UPDATE1)
			skip = mode >> 1;
		for (i = 1 + skip; i < tx->num_desc; i++)
			sdma_unmap_desc(dd, &tx->descp[i]);
		tx->num_desc = 0;
	}
	kfree(tx->coalesce_buf);
	tx->coalesce_buf = NULL;
	/* kmalloc'ed descp */
	if (unlikely(tx->desc_limit > ARRAY_SIZE(tx->descs))) {
		tx->desc_limit = ARRAY_SIZE(tx->descs);
		kfree(tx->descp);
	}
}