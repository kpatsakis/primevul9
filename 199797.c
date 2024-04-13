static int __ipv6_isatap_ifid(u8 *eui, __be32 addr)
{
	if (addr == 0)
		return -1;
	eui[0] = (ipv4_is_zeronet(addr) || ipv4_is_private_10(addr) ||
		  ipv4_is_loopback(addr) || ipv4_is_linklocal_169(addr) ||
		  ipv4_is_private_172(addr) || ipv4_is_test_192(addr) ||
		  ipv4_is_anycast_6to4(addr) || ipv4_is_private_192(addr) ||
		  ipv4_is_test_198(addr) || ipv4_is_multicast(addr) ||
		  ipv4_is_lbcast(addr)) ? 0x00 : 0x02;
	eui[1] = 0;
	eui[2] = 0x5E;
	eui[3] = 0xFE;
	memcpy(eui + 4, &addr, 4);
	return 0;
}