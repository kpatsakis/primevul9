static u8 *pcan_msg_init_empty(struct pcan_usb_pro_msg *pm,
			       void *buffer_addr, int buffer_size)
{
	u8 *pr = pcan_msg_init(pm, buffer_addr, buffer_size);

	if (pr) {
		pm->rec_buffer_len = PCAN_USBPRO_MSG_HEADER_LEN;
		*pm->u.rec_cnt = 0;
	}
	return pr;
}