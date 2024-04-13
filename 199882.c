size_t snd_dma_get_reserved_buf(struct snd_dma_buffer *dmab, unsigned int id)
{
	struct snd_mem_list *mem;

	snd_assert(dmab, return 0);

	mutex_lock(&list_mutex);
	list_for_each_entry(mem, &mem_list_head, list) {
		if (mem->id == id &&
		    (mem->buffer.dev.dev == NULL || dmab->dev.dev == NULL ||
		     ! memcmp(&mem->buffer.dev, &dmab->dev, sizeof(dmab->dev)))) {
			struct device *dev = dmab->dev.dev;
			list_del(&mem->list);
			*dmab = mem->buffer;
			if (dmab->dev.dev == NULL)
				dmab->dev.dev = dev;
			kfree(mem);
			mutex_unlock(&list_mutex);
			return dmab->bytes;
		}
	}
	mutex_unlock(&list_mutex);
	return 0;
}