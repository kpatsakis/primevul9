static inline u8 ahg_mode(struct sdma_txreq *tx)
{
	return (tx->descp[0].qw[1] & SDMA_DESC1_HEADER_MODE_SMASK)
		>> SDMA_DESC1_HEADER_MODE_SHIFT;
}