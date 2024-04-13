static struct mdesc_handle *mdesc_alloc(unsigned int mdesc_size,
					struct mdesc_mem_ops *mops)
{
	struct mdesc_handle *hp = mops->alloc(mdesc_size);

	if (hp)
		hp->mops = mops;

	return hp;
}