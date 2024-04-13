rb_allocate_cpu_buffer(struct ring_buffer *buffer, long nr_pages, int cpu)
{
	struct ring_buffer_per_cpu *cpu_buffer;
	struct buffer_page *bpage;
	struct page *page;
	int ret;

	cpu_buffer = kzalloc_node(ALIGN(sizeof(*cpu_buffer), cache_line_size()),
				  GFP_KERNEL, cpu_to_node(cpu));
	if (!cpu_buffer)
		return NULL;

	cpu_buffer->cpu = cpu;
	cpu_buffer->buffer = buffer;
	raw_spin_lock_init(&cpu_buffer->reader_lock);
	lockdep_set_class(&cpu_buffer->reader_lock, buffer->reader_lock_key);
	cpu_buffer->lock = (arch_spinlock_t)__ARCH_SPIN_LOCK_UNLOCKED;
	INIT_WORK(&cpu_buffer->update_pages_work, update_pages_handler);
	init_completion(&cpu_buffer->update_done);
	init_irq_work(&cpu_buffer->irq_work.work, rb_wake_up_waiters);
	init_waitqueue_head(&cpu_buffer->irq_work.waiters);
	init_waitqueue_head(&cpu_buffer->irq_work.full_waiters);

	bpage = kzalloc_node(ALIGN(sizeof(*bpage), cache_line_size()),
			    GFP_KERNEL, cpu_to_node(cpu));
	if (!bpage)
		goto fail_free_buffer;

	rb_check_bpage(cpu_buffer, bpage);

	cpu_buffer->reader_page = bpage;
	page = alloc_pages_node(cpu_to_node(cpu), GFP_KERNEL, 0);
	if (!page)
		goto fail_free_reader;
	bpage->page = page_address(page);
	rb_init_page(bpage->page);

	INIT_LIST_HEAD(&cpu_buffer->reader_page->list);
	INIT_LIST_HEAD(&cpu_buffer->new_pages);

	ret = rb_allocate_pages(cpu_buffer, nr_pages);
	if (ret < 0)
		goto fail_free_reader;

	cpu_buffer->head_page
		= list_entry(cpu_buffer->pages, struct buffer_page, list);
	cpu_buffer->tail_page = cpu_buffer->commit_page = cpu_buffer->head_page;

	rb_head_page_activate(cpu_buffer);

	return cpu_buffer;

 fail_free_reader:
	free_buffer_page(cpu_buffer->reader_page);

 fail_free_buffer:
	kfree(cpu_buffer);
	return NULL;
}