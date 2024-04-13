void proto_unregister(struct proto *prot)
{
	mutex_lock(&proto_list_mutex);
	release_proto_idx(prot);
	list_del(&prot->node);
	mutex_unlock(&proto_list_mutex);

	kmem_cache_destroy(prot->slab);
	prot->slab = NULL;

	req_prot_cleanup(prot->rsk_prot);

	if (prot->twsk_prot != NULL && prot->twsk_prot->twsk_slab != NULL) {
		kmem_cache_destroy(prot->twsk_prot->twsk_slab);
		kfree(prot->twsk_prot->twsk_slab_name);
		prot->twsk_prot->twsk_slab = NULL;
	}
}