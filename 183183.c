void exit_aio(struct mm_struct *mm)
{
	struct kioctx_table *table;
	struct kioctx *ctx;
	unsigned i = 0;

	while (1) {
		struct completion requests_done =
			COMPLETION_INITIALIZER_ONSTACK(requests_done);

		rcu_read_lock();
		table = rcu_dereference(mm->ioctx_table);

		do {
			if (!table || i >= table->nr) {
				rcu_read_unlock();
				rcu_assign_pointer(mm->ioctx_table, NULL);
				if (table)
					kfree(table);
				return;
			}

			ctx = table->table[i++];
		} while (!ctx);

		rcu_read_unlock();

		/*
		 * We don't need to bother with munmap() here -
		 * exit_mmap(mm) is coming and it'll unmap everything.
		 * Since aio_free_ring() uses non-zero ->mmap_size
		 * as indicator that it needs to unmap the area,
		 * just set it to 0; aio_free_ring() is the only
		 * place that uses ->mmap_size, so it's safe.
		 */
		ctx->mmap_size = 0;

		kill_ioctx(mm, ctx, &requests_done);

		/* Wait until all IO for the context are done. */
		wait_for_completion(&requests_done);
	}
}