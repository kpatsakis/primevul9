static u32 llsec_dev_hash_short(__le16 short_addr, __le16 pan_id)
{
	return ((__force u16)short_addr) << 16 | (__force u16)pan_id;
}