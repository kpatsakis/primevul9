static int eb_reserve(struct i915_execbuffer *eb)
{
	const unsigned int count = eb->buffer_count;
	struct list_head last;
	struct i915_vma *vma;
	unsigned int i, pass;
	int err;

	/*
	 * Attempt to pin all of the buffers into the GTT.
	 * This is done in 3 phases:
	 *
	 * 1a. Unbind all objects that do not match the GTT constraints for
	 *     the execbuffer (fenceable, mappable, alignment etc).
	 * 1b. Increment pin count for already bound objects.
	 * 2.  Bind new objects.
	 * 3.  Decrement pin count.
	 *
	 * This avoid unnecessary unbinding of later objects in order to make
	 * room for the earlier objects *unless* we need to defragment.
	 */

	pass = 0;
	err = 0;
	do {
		list_for_each_entry(vma, &eb->unbound, exec_link) {
			err = eb_reserve_vma(eb, vma);
			if (err)
				break;
		}
		if (err != -ENOSPC)
			return err;

		/* Resort *all* the objects into priority order */
		INIT_LIST_HEAD(&eb->unbound);
		INIT_LIST_HEAD(&last);
		for (i = 0; i < count; i++) {
			unsigned int flags = eb->flags[i];
			struct i915_vma *vma = eb->vma[i];

			if (flags & EXEC_OBJECT_PINNED &&
			    flags & __EXEC_OBJECT_HAS_PIN)
				continue;

			eb_unreserve_vma(vma, &eb->flags[i]);

			if (flags & EXEC_OBJECT_PINNED)
				/* Pinned must have their slot */
				list_add(&vma->exec_link, &eb->unbound);
			else if (flags & __EXEC_OBJECT_NEEDS_MAP)
				/* Map require the lowest 256MiB (aperture) */
				list_add_tail(&vma->exec_link, &eb->unbound);
			else if (!(flags & EXEC_OBJECT_SUPPORTS_48B_ADDRESS))
				/* Prioritise 4GiB region for restricted bo */
				list_add(&vma->exec_link, &last);
			else
				list_add_tail(&vma->exec_link, &last);
		}
		list_splice_tail(&last, &eb->unbound);

		switch (pass++) {
		case 0:
			break;

		case 1:
			/* Too fragmented, unbind everything and retry */
			err = i915_gem_evict_vm(eb->vm);
			if (err)
				return err;
			break;

		default:
			return -ENOSPC;
		}
	} while (1);
}