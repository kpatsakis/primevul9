static bool valid_packet_size(size_t len)
{
	/*
	 * A WRITEX with CAP_LARGE_WRITEX can be 64k worth of data plus 65 bytes
	 * of header. Don't print the error if this fits.... JRA.
	 */

	if (len > (BUFFER_SIZE + LARGE_WRITEX_HDR_SIZE)) {
		DEBUG(0,("Invalid packet length! (%lu bytes).\n",
					(unsigned long)len));
		return false;
	}
	return true;
}