static bool pn533_acr122_is_rx_frame_valid(void *_frame, struct pn533 *dev)
{
	struct pn533_acr122_rx_frame *frame = _frame;

	if (frame->ccid.type != 0x83)
		return false;

	if (!frame->ccid.datalen)
		return false;

	if (frame->data[frame->ccid.datalen - 2] == 0x63)
		return false;

	return true;
}