static int ttusb_dec_get_stb_state (struct ttusb_dec *dec, unsigned int *mode,
				    unsigned int *model, unsigned int *version)
{
	u8 c[COMMAND_PACKET_SIZE];
	int c_length;
	int result;
	__be32 tmp;

	dprintk("%s\n", __func__);

	result = ttusb_dec_send_command(dec, 0x08, 0, NULL, &c_length, c);
	if (result)
		return result;

	if (c_length >= 0x0c) {
		if (mode != NULL) {
			memcpy(&tmp, c, 4);
			*mode = ntohl(tmp);
		}
		if (model != NULL) {
			memcpy(&tmp, &c[4], 4);
			*model = ntohl(tmp);
		}
		if (version != NULL) {
			memcpy(&tmp, &c[8], 4);
			*version = ntohl(tmp);
		}
		return 0;
	} else {
		return -ENOENT;
	}
}