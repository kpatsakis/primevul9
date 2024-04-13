resp_addr_get_rrset(struct resp_addr* addr)
{
	return addr ? addr->data : NULL;
}