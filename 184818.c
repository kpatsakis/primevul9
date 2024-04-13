static struct frag_queue *fq_find(struct net *net, __be32 id, u32 user,
				  const struct ipv6hdr *hdr, int iif)
{
	struct frag_v6_compare_key key = {
		.id = id,
		.saddr = hdr->saddr,
		.daddr = hdr->daddr,
		.user = user,
		.iif = iif,
	};
	struct inet_frag_queue *q;

	q = inet_frag_find(&net->nf_frag.frags, &key);
	if (!q)
		return NULL;

	return container_of(q, struct frag_queue, q);
}