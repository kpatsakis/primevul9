static int req_prot_init(const struct proto *prot)
{
	struct request_sock_ops *rsk_prot = prot->rsk_prot;

	if (!rsk_prot)
		return 0;

	rsk_prot->slab_name = kasprintf(GFP_KERNEL, "request_sock_%s",
					prot->name);
	if (!rsk_prot->slab_name)
		return -ENOMEM;

	rsk_prot->slab = kmem_cache_create(rsk_prot->slab_name,
					   rsk_prot->obj_size, 0,
					   prot->slab_flags, NULL);

	if (!rsk_prot->slab) {
		pr_crit("%s: Can't create request sock SLAB cache!\n",
			prot->name);
		return -ENOMEM;
	}
	return 0;
}