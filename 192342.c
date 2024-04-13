static inline void nf_conntrack_get(struct nf_conntrack *nfct)
{
	if (nfct)
		atomic_inc(&nfct->use);
}