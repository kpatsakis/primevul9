relocate_entry(struct i915_vma *vma,
	       const struct drm_i915_gem_relocation_entry *reloc,
	       struct i915_execbuffer *eb,
	       const struct i915_vma *target)
{
	u64 offset = reloc->offset;
	u64 target_offset = relocation_target(reloc, target);
	bool wide = eb->reloc_cache.use_64bit_reloc;
	void *vaddr;

	if (!eb->reloc_cache.vaddr &&
	    (DBG_FORCE_RELOC == FORCE_GPU_RELOC ||
	     !reservation_object_test_signaled_rcu(vma->resv, true))) {
		const unsigned int gen = eb->reloc_cache.gen;
		unsigned int len;
		u32 *batch;
		u64 addr;

		if (wide)
			len = offset & 7 ? 8 : 5;
		else if (gen >= 4)
			len = 4;
		else
			len = 3;

		batch = reloc_gpu(eb, vma, len);
		if (IS_ERR(batch))
			goto repeat;

		addr = gen8_canonical_addr(vma->node.start + offset);
		if (wide) {
			if (offset & 7) {
				*batch++ = MI_STORE_DWORD_IMM_GEN4;
				*batch++ = lower_32_bits(addr);
				*batch++ = upper_32_bits(addr);
				*batch++ = lower_32_bits(target_offset);

				addr = gen8_canonical_addr(addr + 4);

				*batch++ = MI_STORE_DWORD_IMM_GEN4;
				*batch++ = lower_32_bits(addr);
				*batch++ = upper_32_bits(addr);
				*batch++ = upper_32_bits(target_offset);
			} else {
				*batch++ = (MI_STORE_DWORD_IMM_GEN4 | (1 << 21)) + 1;
				*batch++ = lower_32_bits(addr);
				*batch++ = upper_32_bits(addr);
				*batch++ = lower_32_bits(target_offset);
				*batch++ = upper_32_bits(target_offset);
			}
		} else if (gen >= 6) {
			*batch++ = MI_STORE_DWORD_IMM_GEN4;
			*batch++ = 0;
			*batch++ = addr;
			*batch++ = target_offset;
		} else if (gen >= 4) {
			*batch++ = MI_STORE_DWORD_IMM_GEN4 | MI_USE_GGTT;
			*batch++ = 0;
			*batch++ = addr;
			*batch++ = target_offset;
		} else {
			*batch++ = MI_STORE_DWORD_IMM | MI_MEM_VIRTUAL;
			*batch++ = addr;
			*batch++ = target_offset;
		}

		goto out;
	}

repeat:
	vaddr = reloc_vaddr(vma->obj, &eb->reloc_cache, offset >> PAGE_SHIFT);
	if (IS_ERR(vaddr))
		return PTR_ERR(vaddr);

	clflush_write32(vaddr + offset_in_page(offset),
			lower_32_bits(target_offset),
			eb->reloc_cache.vaddr);

	if (wide) {
		offset += sizeof(u32);
		target_offset >>= 32;
		wide = false;
		goto repeat;
	}

out:
	return target->node.start | UPDATE;
}