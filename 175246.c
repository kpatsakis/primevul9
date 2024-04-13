void __init skb_init(void)
{
	skbuff_head_cache = kmem_cache_create("skbuff_head_cache",
					      sizeof(struct sk_buff),
					      0,
					      SLAB_HWCACHE_ALIGN|SLAB_PANIC,
					      NULL);
	skbuff_fclone_cache = kmem_cache_create("skbuff_fclone_cache",
						(2*sizeof(struct sk_buff)) +
						sizeof(atomic_t),
						0,
						SLAB_HWCACHE_ALIGN|SLAB_PANIC,
						NULL);
}