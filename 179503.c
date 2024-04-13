display_signed_time(gchar *buf, int buflen, const gint64 sec, gint32 frac,
		const to_str_time_res_t units)
{
	/* this buffer is not NUL terminated */
	gint8 num_buf[CHARS_64_BIT_SIGNED];
	gint8 *num_end = &num_buf[CHARS_64_BIT_SIGNED];
	gint8 *num_ptr;
	int num_len;

	if (buflen < 1)
		return;

	/* If the fractional part of the time stamp is negative,
	   print its absolute value and, if the seconds part isn't
	   (the seconds part should be zero in that case), stick
	   a "-" in front of the entire time stamp. */
	if (frac < 0) {
		frac = -frac;
		if (sec >= 0) {
			buf[0] = '-';
			buf++;
			buflen--;
		}
	}

	num_ptr = int64_to_str_back(num_end, sec);

	num_len = MIN((int) (num_end - num_ptr), buflen);
	memcpy(buf, num_ptr, num_len);
	buf += num_len;
	buflen -= num_len;

	switch (units) {
		case TO_STR_TIME_RES_T_SECS:
		default:
			/* no fraction */
			num_ptr = NULL;
			break;

		case TO_STR_TIME_RES_T_DSECS:
			num_ptr = uint_to_str_back_len(num_end, frac, 1);
			break;

		case TO_STR_TIME_RES_T_CSECS:
			num_ptr = uint_to_str_back_len(num_end, frac, 2);
			break;

		case TO_STR_TIME_RES_T_MSECS:
			num_ptr = uint_to_str_back_len(num_end, frac, 3);
			break;

		case TO_STR_TIME_RES_T_USECS:
			num_ptr = uint_to_str_back_len(num_end, frac, 6);
			break;

		case TO_STR_TIME_RES_T_NSECS:
			num_ptr = uint_to_str_back_len(num_end, frac, 9);
			break;
	}

	if (num_ptr != NULL)
	{
		*(--num_ptr) = '.';

		num_len = MIN((int) (num_end - num_ptr), buflen);
		memcpy(buf, num_ptr, num_len);
		buf += num_len;
		buflen -= num_len;
	}

	/* need to NUL terminate, we know that buffer had at least 1 byte */
	if (buflen == 0)
		buf--;
	*buf = '\0';
}