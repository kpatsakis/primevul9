static u8 *pcan_msg_init(struct pcan_usb_pro_msg *pm, void *buffer_addr,
			 int buffer_size)
{
	if (buffer_size < PCAN_USBPRO_MSG_HEADER_LEN)
		return NULL;

	pm->u.rec_buffer = (u8 *)buffer_addr;
	pm->rec_buffer_size = pm->rec_buffer_len = buffer_size;
	pm->rec_ptr = pm->u.rec_buffer + PCAN_USBPRO_MSG_HEADER_LEN;

	return pm->rec_ptr;
}