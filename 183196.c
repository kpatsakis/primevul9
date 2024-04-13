static void pn533_acr122_tx_update_payload_len(void *_frame, int len)
{
	struct pn533_acr122_tx_frame *frame = _frame;

	frame->datalen += len;
}