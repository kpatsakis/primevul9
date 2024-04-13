static u64 llsec_dev_hash_long(__le64 hwaddr)
{
	return (__force u64)hwaddr;
}