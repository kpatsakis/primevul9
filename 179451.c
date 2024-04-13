static noinline int eb_relocate_slow(struct i915_execbuffer *eb)
{
	struct drm_device *dev = &eb->i915->drm;
	bool have_copy = false;
	struct i915_vma *vma;
	int err = 0;

repeat:
	if (signal_pending(current)) {
		err = -ERESTARTSYS;
		goto out;
	}

	/* We may process another execbuffer during the unlock... */
	eb_reset_vmas(eb);
	mutex_unlock(&dev->struct_mutex);

	/*
	 * We take 3 passes through the slowpatch.
	 *
	 * 1 - we try to just prefault all the user relocation entries and
	 * then attempt to reuse the atomic pagefault disabled fast path again.
	 *
	 * 2 - we copy the user entries to a local buffer here outside of the
	 * local and allow ourselves to wait upon any rendering before
	 * relocations
	 *
	 * 3 - we already have a local copy of the relocation entries, but
	 * were interrupted (EAGAIN) whilst waiting for the objects, try again.
	 */
	if (!err) {
		err = eb_prefault_relocations(eb);
	} else if (!have_copy) {
		err = eb_copy_relocations(eb);
		have_copy = err == 0;
	} else {
		cond_resched();
		err = 0;
	}
	if (err) {
		mutex_lock(&dev->struct_mutex);
		goto out;
	}

	/* A frequent cause for EAGAIN are currently unavailable client pages */
	flush_workqueue(eb->i915->mm.userptr_wq);

	err = i915_mutex_lock_interruptible(dev);
	if (err) {
		mutex_lock(&dev->struct_mutex);
		goto out;
	}

	/* reacquire the objects */
	err = eb_lookup_vmas(eb);
	if (err)
		goto err;

	GEM_BUG_ON(!eb->batch);

	list_for_each_entry(vma, &eb->relocs, reloc_link) {
		if (!have_copy) {
			pagefault_disable();
			err = eb_relocate_vma(eb, vma);
			pagefault_enable();
			if (err)
				goto repeat;
		} else {
			err = eb_relocate_vma_slow(eb, vma);
			if (err)
				goto err;
		}
	}

	/*
	 * Leave the user relocations as are, this is the painfully slow path,
	 * and we want to avoid the complication of dropping the lock whilst
	 * having buffers reserved in the aperture and so causing spurious
	 * ENOSPC for random operations.
	 */

err:
	if (err == -EAGAIN)
		goto repeat;

out:
	if (have_copy) {
		const unsigned int count = eb->buffer_count;
		unsigned int i;

		for (i = 0; i < count; i++) {
			const struct drm_i915_gem_exec_object2 *entry =
				&eb->exec[i];
			struct drm_i915_gem_relocation_entry *relocs;

			if (!entry->relocation_count)
				continue;

			relocs = u64_to_ptr(typeof(*relocs), entry->relocs_ptr);
			kvfree(relocs);
		}
	}

	return err;
}