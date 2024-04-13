void rndis_set_host_mac(struct rndis_params *params, const u8 *addr)
{
	params->host_mac = addr;
}