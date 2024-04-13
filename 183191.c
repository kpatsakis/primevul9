static void pn533_acr122_tx_frame_finish(void *_frame)
{
	struct pn533_acr122_tx_frame *frame = _frame;

	frame->ccid.datalen += frame->datalen;
}