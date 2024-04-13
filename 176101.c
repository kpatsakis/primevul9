adv_error adv_png_read_iend(adv_fz* f, const unsigned char* data, unsigned data_size, unsigned type)
{
	if (type == ADV_PNG_CN_IEND)
		return 0;

	/* ancillary bit. bit 5 of first byte. 0 (uppercase) = critical, 1 (lowercase) = ancillary. */
	if ((type & 0x20000000) == 0) {
		char buf[4];
		be_uint32_write(buf, type);
		error_unsupported_set("Unsupported critical chunk '%c%c%c%c'", buf[0], buf[1], buf[2], buf[3]);
		return -1;
	}

	while (1) {
		unsigned char* ptr;
		unsigned ptr_size;

		/* read next */
		if (adv_png_read_chunk(f, &ptr, &ptr_size, &type) != 0) {
			return -1;
		}

		free(ptr);

		if (type == ADV_PNG_CN_IEND)
			break;

		/* ancillary bit. bit 5 of first byte. 0 (uppercase) = critical, 1 (lowercase) = ancillary. */
		if ((type & 0x20000000) == 0) {
			char buf[4];
			be_uint32_write(buf, type);
			error_unsupported_set("Unsupported critical chunk '%c%c%c%c'", buf[0], buf[1], buf[2], buf[3]);
			return -1;
		}
	}

	return 0;
}