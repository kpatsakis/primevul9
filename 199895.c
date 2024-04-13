int snd_dma_reserve_buf(struct snd_dma_buffer *dmab, unsigned int id)
{
	struct snd_mem_list *mem;

	snd_assert(dmab, return -EINVAL);
	mem = kmalloc(sizeof(*mem), GFP_KERNEL);
	if (! mem)
		return -ENOMEM;
	mutex_lock(&list_mutex);
	mem->buffer = *dmab;
	mem->id = id;
	list_add_tail(&mem->list, &mem_list_head);
	mutex_unlock(&list_mutex);
	return 0;
}