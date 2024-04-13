static const ut8 *fill_block_data(const ut8 *buf, const ut8 *buf_end, RzBinDwarfBlock *block) {
	block->data = calloc(sizeof(ut8), block->length);
	if (!block->data) {
		return NULL;
	}
	/* Maybe unroll this as an optimization in future? */
	if (block->data) {
		size_t j = 0;
		for (j = 0; j < block->length; j++) {
			block->data[j] = READ8(buf);
		}
	}
	return buf;
}