display_epoch_time(gchar *buf, int buflen, const time_t sec, gint32 frac,
		const to_str_time_res_t units)
{
	double elapsed_secs;

	elapsed_secs = difftime(sec,(time_t)0);

	/* This code copied from display_signed_time; keep it in case anyone
	   is looking at captures from before 1970 (???).
	   If the fractional part of the time stamp is negative,
	   print its absolute value and, if the seconds part isn't
	   (the seconds part should be zero in that case), stick
	   a "-" in front of the entire time stamp. */
	if (frac < 0) {
		frac = -frac;
		if (elapsed_secs >= 0) {
			if (buflen < 1) {
				return;
			}
			buf[0] = '-';
			buf++;
			buflen--;
		}
	}
	switch (units) {

		case TO_STR_TIME_RES_T_SECS:
			g_snprintf(buf, buflen, "%0.0f", elapsed_secs);
			break;

		case TO_STR_TIME_RES_T_DSECS:
			g_snprintf(buf, buflen, "%0.0f.%01d", elapsed_secs, frac);
			break;

		case TO_STR_TIME_RES_T_CSECS:
			g_snprintf(buf, buflen, "%0.0f.%02d", elapsed_secs, frac);
			break;

		case TO_STR_TIME_RES_T_MSECS:
			g_snprintf(buf, buflen, "%0.0f.%03d", elapsed_secs, frac);
			break;

		case TO_STR_TIME_RES_T_USECS:
			g_snprintf(buf, buflen, "%0.0f.%06d", elapsed_secs, frac);
			break;

		case TO_STR_TIME_RES_T_NSECS:
			g_snprintf(buf, buflen, "%0.0f.%09d", elapsed_secs, frac);
			break;
	}
}