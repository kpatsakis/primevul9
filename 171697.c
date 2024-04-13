static char *sock_prot_memory_pressure(struct proto *proto)
{
	return proto->memory_pressure != NULL ?
	proto_memory_pressure(proto) ? "yes" : "no" : "NI";
}