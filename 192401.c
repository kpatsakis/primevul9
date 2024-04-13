static inline void nf_bridge_get(struct nf_bridge_info *nf_bridge)
{
	if (nf_bridge)
		atomic_inc(&nf_bridge->use);
}