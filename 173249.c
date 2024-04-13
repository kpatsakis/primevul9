static void bnx2x_set_mac_buf(u8 *mac_buf, u32 mac_lo, u16 mac_hi)
{
	__be16 mac_hi_be = cpu_to_be16(mac_hi);
	__be32 mac_lo_be = cpu_to_be32(mac_lo);
	memcpy(mac_buf, &mac_hi_be, sizeof(mac_hi_be));
	memcpy(mac_buf + sizeof(mac_hi_be), &mac_lo_be, sizeof(mac_lo_be));
}