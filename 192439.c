void make_flow_keys_digest(struct flow_keys_digest *digest,
			   const struct flow_keys *flow)
{
	struct _flow_keys_digest_data *data =
	    (struct _flow_keys_digest_data *)digest;

	BUILD_BUG_ON(sizeof(*data) > sizeof(*digest));

	memset(digest, 0, sizeof(*digest));

	data->n_proto = flow->basic.n_proto;
	data->ip_proto = flow->basic.ip_proto;
	data->ports = flow->ports.ports;
	data->src = flow->addrs.v4addrs.src;
	data->dst = flow->addrs.v4addrs.dst;
}