static void assign_proto_idx(struct proto *prot)
{
	prot->inuse_idx = find_first_zero_bit(proto_inuse_idx, PROTO_INUSE_NR);

	if (unlikely(prot->inuse_idx == PROTO_INUSE_NR - 1)) {
		pr_err("PROTO_INUSE_NR exhausted\n");
		return;
	}

	set_bit(prot->inuse_idx, proto_inuse_idx);
}