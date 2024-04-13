static int __init userfaultfd_init(void)
{
	userfaultfd_ctx_cachep = kmem_cache_create("userfaultfd_ctx_cache",
						sizeof(struct userfaultfd_ctx),
						0,
						SLAB_HWCACHE_ALIGN|SLAB_PANIC,
						init_once_userfaultfd_ctx);
	return 0;
}