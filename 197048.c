static u32 bond_sk_hash_l34(struct sock *sk)
{
	struct flow_keys flow;
	u32 hash;

	bond_sk_to_flow(sk, &flow);

	/* L4 */
	memcpy(&hash, &flow.ports.ports, sizeof(hash));
	/* L3 */
	return bond_ip_hash(hash, &flow);
}