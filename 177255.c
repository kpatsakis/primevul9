static void sdma_rht_free(void *ptr, void *arg)
{
	struct sdma_rht_node *rht_node = ptr;
	int i;

	for (i = 0; i < HFI1_MAX_VLS_SUPPORTED; i++)
		kfree(rht_node->map[i]);

	kfree(rht_node);
}