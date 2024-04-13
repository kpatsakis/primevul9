static bool llsec_dev_use_shortaddr(__le16 short_addr)
{
	return short_addr != cpu_to_le16(IEEE802154_ADDR_UNDEF) &&
		short_addr != cpu_to_le16(0xffff);
}