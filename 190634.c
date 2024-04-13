int __init f2fs_init_post_read_processing(void)
{
	bio_post_read_ctx_cache = KMEM_CACHE(bio_post_read_ctx, 0);
	if (!bio_post_read_ctx_cache)
		goto fail;
	bio_post_read_ctx_pool =
		mempool_create_slab_pool(NUM_PREALLOC_POST_READ_CTXS,
					 bio_post_read_ctx_cache);
	if (!bio_post_read_ctx_pool)
		goto fail_free_cache;
	return 0;

fail_free_cache:
	kmem_cache_destroy(bio_post_read_ctx_cache);
fail:
	return -ENOMEM;
}