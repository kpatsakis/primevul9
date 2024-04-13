static u8 mwifiex_get_random_ba_threshold(void)
{
	u64 ns;
	/* setup ba_packet_threshold here random number between
	 * [BA_SETUP_PACKET_OFFSET,
	 * BA_SETUP_PACKET_OFFSET+BA_SETUP_MAX_PACKET_THRESHOLD-1]
	 */
	ns = ktime_get_ns();
	ns += (ns >> 32) + (ns >> 16);

	return ((u8)ns % BA_SETUP_MAX_PACKET_THRESHOLD) + BA_SETUP_PACKET_OFFSET;
}