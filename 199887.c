static void __exit snd_mem_exit(void)
{
	remove_proc_entry(SND_MEM_PROC_FILE, NULL);
	free_all_reserved_pages();
	if (snd_allocated_pages > 0)
		printk(KERN_ERR "snd-malloc: Memory leak?  pages not freed = %li\n", snd_allocated_pages);
}