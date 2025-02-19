static void free_all_reserved_pages(void)
{
	struct list_head *p;
	struct snd_mem_list *mem;

	mutex_lock(&list_mutex);
	while (! list_empty(&mem_list_head)) {
		p = mem_list_head.next;
		mem = list_entry(p, struct snd_mem_list, list);
		list_del(p);
		snd_dma_free_pages(&mem->buffer);
		kfree(mem);
	}
	mutex_unlock(&list_mutex);
}