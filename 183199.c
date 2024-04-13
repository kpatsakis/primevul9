static u8 pn533_acr122_get_cmd_code(void *frame)
{
	struct pn533_acr122_rx_frame *f = frame;

	return PN533_FRAME_CMD(f);
}