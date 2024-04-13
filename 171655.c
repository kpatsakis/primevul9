int proto_register(struct proto *prot, int alloc_slab)
{
	if (alloc_slab) {
		prot->slab = kmem_cache_create(prot->name, prot->obj_size, 0,
					SLAB_HWCACHE_ALIGN | prot->slab_flags,
					NULL);

		if (prot->slab == NULL) {
			pr_crit("%s: Can't create sock SLAB cache!\n",
				prot->name);
			goto out;
		}

		if (req_prot_init(prot))
			goto out_free_request_sock_slab;

		if (prot->twsk_prot != NULL) {
			prot->twsk_prot->twsk_slab_name = kasprintf(GFP_KERNEL, "tw_sock_%s", prot->name);

			if (prot->twsk_prot->twsk_slab_name == NULL)
				goto out_free_request_sock_slab;

			prot->twsk_prot->twsk_slab =
				kmem_cache_create(prot->twsk_prot->twsk_slab_name,
						  prot->twsk_prot->twsk_obj_size,
						  0,
						  prot->slab_flags,
						  NULL);
			if (prot->twsk_prot->twsk_slab == NULL)
				goto out_free_timewait_sock_slab_name;
		}
	}

	mutex_lock(&proto_list_mutex);
	list_add(&prot->node, &proto_list);
	assign_proto_idx(prot);
	mutex_unlock(&proto_list_mutex);
	return 0;

out_free_timewait_sock_slab_name:
	kfree(prot->twsk_prot->twsk_slab_name);
out_free_request_sock_slab:
	req_prot_cleanup(prot->rsk_prot);

	kmem_cache_destroy(prot->slab);
	prot->slab = NULL;
out:
	return -ENOBUFS;
}