int __kvm_set_memory_region(struct kvm *kvm,
			    const struct kvm_userspace_memory_region *mem)
{
	struct kvm_memory_slot old, new;
	struct kvm_memory_slot *tmp;
	enum kvm_mr_change change;
	int as_id, id;
	int r;

	r = check_memory_region_flags(mem);
	if (r)
		return r;

	as_id = mem->slot >> 16;
	id = (u16)mem->slot;

	/* General sanity checks */
	if (mem->memory_size & (PAGE_SIZE - 1))
		return -EINVAL;
	if (mem->guest_phys_addr & (PAGE_SIZE - 1))
		return -EINVAL;
	/* We can read the guest memory with __xxx_user() later on. */
	if ((mem->userspace_addr & (PAGE_SIZE - 1)) ||
	     !access_ok((void __user *)(unsigned long)mem->userspace_addr,
			mem->memory_size))
		return -EINVAL;
	if (as_id >= KVM_ADDRESS_SPACE_NUM || id >= KVM_MEM_SLOTS_NUM)
		return -EINVAL;
	if (mem->guest_phys_addr + mem->memory_size < mem->guest_phys_addr)
		return -EINVAL;

	/*
	 * Make a full copy of the old memslot, the pointer will become stale
	 * when the memslots are re-sorted by update_memslots(), and the old
	 * memslot needs to be referenced after calling update_memslots(), e.g.
	 * to free its resources and for arch specific behavior.
	 */
	tmp = id_to_memslot(__kvm_memslots(kvm, as_id), id);
	if (tmp) {
		old = *tmp;
		tmp = NULL;
	} else {
		memset(&old, 0, sizeof(old));
		old.id = id;
	}

	if (!mem->memory_size)
		return kvm_delete_memslot(kvm, mem, &old, as_id);

	new.id = id;
	new.base_gfn = mem->guest_phys_addr >> PAGE_SHIFT;
	new.npages = mem->memory_size >> PAGE_SHIFT;
	new.flags = mem->flags;
	new.userspace_addr = mem->userspace_addr;

	if (new.npages > KVM_MEM_MAX_NR_PAGES)
		return -EINVAL;

	if (!old.npages) {
		change = KVM_MR_CREATE;
		new.dirty_bitmap = NULL;
		memset(&new.arch, 0, sizeof(new.arch));
	} else { /* Modify an existing slot. */
		if ((new.userspace_addr != old.userspace_addr) ||
		    (new.npages != old.npages) ||
		    ((new.flags ^ old.flags) & KVM_MEM_READONLY))
			return -EINVAL;

		if (new.base_gfn != old.base_gfn)
			change = KVM_MR_MOVE;
		else if (new.flags != old.flags)
			change = KVM_MR_FLAGS_ONLY;
		else /* Nothing to change. */
			return 0;

		/* Copy dirty_bitmap and arch from the current memslot. */
		new.dirty_bitmap = old.dirty_bitmap;
		memcpy(&new.arch, &old.arch, sizeof(new.arch));
	}

	if ((change == KVM_MR_CREATE) || (change == KVM_MR_MOVE)) {
		/* Check for overlaps */
		kvm_for_each_memslot(tmp, __kvm_memslots(kvm, as_id)) {
			if (tmp->id == id)
				continue;
			if (!((new.base_gfn + new.npages <= tmp->base_gfn) ||
			      (new.base_gfn >= tmp->base_gfn + tmp->npages)))
				return -EEXIST;
		}
	}

	/* Allocate/free page dirty bitmap as needed */
	if (!(new.flags & KVM_MEM_LOG_DIRTY_PAGES))
		new.dirty_bitmap = NULL;
	else if (!new.dirty_bitmap) {
		r = kvm_alloc_dirty_bitmap(&new);
		if (r)
			return r;

		if (kvm_dirty_log_manual_protect_and_init_set(kvm))
			bitmap_set(new.dirty_bitmap, 0, new.npages);
	}

	r = kvm_set_memslot(kvm, mem, &old, &new, as_id, change);
	if (r)
		goto out_bitmap;

	if (old.dirty_bitmap && !new.dirty_bitmap)
		kvm_destroy_dirty_bitmap(&old);
	return 0;

out_bitmap:
	if (new.dirty_bitmap && !old.dirty_bitmap)
		kvm_destroy_dirty_bitmap(&new);
	return r;
}