static const char *i40e_tunnel_name(u8 type)
{
	switch (type) {
	case UDP_TUNNEL_TYPE_VXLAN:
		return "vxlan";
	case UDP_TUNNEL_TYPE_GENEVE:
		return "geneve";
	default:
		return "unknown";
	}
}