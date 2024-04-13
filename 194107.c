int __init blk_dev_init(void)
{
	BUILD_BUG_ON(REQ_OP_LAST >= (1 << REQ_OP_BITS));
	BUILD_BUG_ON(REQ_OP_BITS + REQ_FLAG_BITS > 8 *
			FIELD_SIZEOF(struct request, cmd_flags));
	BUILD_BUG_ON(REQ_OP_BITS + REQ_FLAG_BITS > 8 *
			FIELD_SIZEOF(struct bio, bi_opf));

	/* used for unplugging and affects IO latency/throughput - HIGHPRI */
	kblockd_workqueue = alloc_workqueue("kblockd",
					    WQ_MEM_RECLAIM | WQ_HIGHPRI, 0);
	if (!kblockd_workqueue)
		panic("Failed to create kblockd\n");

	blk_requestq_cachep = kmem_cache_create("request_queue",
			sizeof(struct request_queue), 0, SLAB_PANIC, NULL);

#ifdef CONFIG_DEBUG_FS
	blk_debugfs_root = debugfs_create_dir("block", NULL);
#endif

	return 0;
}