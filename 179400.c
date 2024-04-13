static int eb_create(struct i915_execbuffer *eb)
{
	if (!(eb->args->flags & I915_EXEC_HANDLE_LUT)) {
		unsigned int size = 1 + ilog2(eb->buffer_count);

		/*
		 * Without a 1:1 association between relocation handles and
		 * the execobject[] index, we instead create a hashtable.
		 * We size it dynamically based on available memory, starting
		 * first with 1:1 assocative hash and scaling back until
		 * the allocation succeeds.
		 *
		 * Later on we use a positive lut_size to indicate we are
		 * using this hashtable, and a negative value to indicate a
		 * direct lookup.
		 */
		do {
			gfp_t flags;

			/* While we can still reduce the allocation size, don't
			 * raise a warning and allow the allocation to fail.
			 * On the last pass though, we want to try as hard
			 * as possible to perform the allocation and warn
			 * if it fails.
			 */
			flags = GFP_KERNEL;
			if (size > 1)
				flags |= __GFP_NORETRY | __GFP_NOWARN;

			eb->buckets = kzalloc(sizeof(struct hlist_head) << size,
					      flags);
			if (eb->buckets)
				break;
		} while (--size);

		if (unlikely(!size))
			return -ENOMEM;

		eb->lut_size = size;
	} else {
		eb->lut_size = -eb->buffer_count;
	}

	return 0;
}