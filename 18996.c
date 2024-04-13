resp_addr_get_action(const struct resp_addr* addr)
{
	return addr ? addr->action : respip_none;
}