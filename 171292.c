vhost_user_set_mem_table(struct virtio_net *dev, struct VhostUserMsg *pmsg)
{
	struct VhostUserMemory memory = pmsg->payload.memory;
	struct rte_vhost_mem_region *reg;
	void *mmap_addr;
	uint64_t mmap_size;
	uint64_t mmap_offset;
	uint64_t alignment;
	uint32_t i;
	int fd;

	if (memory.nregions > VHOST_MEMORY_MAX_NREGIONS) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"too many memory regions (%u)\n", memory.nregions);
		return -1;
	}

	if (dev->mem && !vhost_memory_changed(&memory, dev->mem)) {
		RTE_LOG(INFO, VHOST_CONFIG,
			"(%d) memory regions not changed\n", dev->vid);

		for (i = 0; i < memory.nregions; i++)
			close(pmsg->fds[i]);

		return 0;
	}

	if (dev->mem) {
		free_mem_region(dev);
		rte_free(dev->mem);
		dev->mem = NULL;
	}

	dev->nr_guest_pages = 0;
	if (!dev->guest_pages) {
		dev->max_guest_pages = 8;
		dev->guest_pages = malloc(dev->max_guest_pages *
						sizeof(struct guest_page));
		if (dev->guest_pages == NULL) {
			RTE_LOG(ERR, VHOST_CONFIG,
				"(%d) failed to allocate memory "
				"for dev->guest_pages\n",
				dev->vid);
			return -1;
		}
	}

	dev->mem = rte_zmalloc("vhost-mem-table", sizeof(struct rte_vhost_memory) +
		sizeof(struct rte_vhost_mem_region) * memory.nregions, 0);
	if (dev->mem == NULL) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"(%d) failed to allocate memory for dev->mem\n",
			dev->vid);
		return -1;
	}
	dev->mem->nregions = memory.nregions;

	for (i = 0; i < memory.nregions; i++) {
		fd  = pmsg->fds[i];
		reg = &dev->mem->regions[i];

		reg->guest_phys_addr = memory.regions[i].guest_phys_addr;
		reg->guest_user_addr = memory.regions[i].userspace_addr;
		reg->size            = memory.regions[i].memory_size;
		reg->fd              = fd;

		mmap_offset = memory.regions[i].mmap_offset;

		/* Check for memory_size + mmap_offset overflow */
		if (mmap_offset >= -reg->size) {
			RTE_LOG(ERR, VHOST_CONFIG,
				"mmap_offset (%#"PRIx64") and memory_size "
				"(%#"PRIx64") overflow\n",
				mmap_offset, reg->size);
			goto err_mmap;
		}

		mmap_size = reg->size + mmap_offset;

		/* mmap() without flag of MAP_ANONYMOUS, should be called
		 * with length argument aligned with hugepagesz at older
		 * longterm version Linux, like 2.6.32 and 3.2.72, or
		 * mmap() will fail with EINVAL.
		 *
		 * to avoid failure, make sure in caller to keep length
		 * aligned.
		 */
		alignment = get_blk_size(fd);
		if (alignment == (uint64_t)-1) {
			RTE_LOG(ERR, VHOST_CONFIG,
				"couldn't get hugepage size through fstat\n");
			goto err_mmap;
		}
		mmap_size = RTE_ALIGN_CEIL(mmap_size, alignment);

		mmap_addr = mmap(NULL, mmap_size, PROT_READ | PROT_WRITE,
				 MAP_SHARED | MAP_POPULATE, fd, 0);

		if (mmap_addr == MAP_FAILED) {
			RTE_LOG(ERR, VHOST_CONFIG,
				"mmap region %u failed.\n", i);
			goto err_mmap;
		}

		reg->mmap_addr = mmap_addr;
		reg->mmap_size = mmap_size;
		reg->host_user_addr = (uint64_t)(uintptr_t)mmap_addr +
				      mmap_offset;

		if (dev->dequeue_zero_copy)
			add_guest_pages(dev, reg, alignment);

		RTE_LOG(INFO, VHOST_CONFIG,
			"guest memory region %u, size: 0x%" PRIx64 "\n"
			"\t guest physical addr: 0x%" PRIx64 "\n"
			"\t guest virtual  addr: 0x%" PRIx64 "\n"
			"\t host  virtual  addr: 0x%" PRIx64 "\n"
			"\t mmap addr : 0x%" PRIx64 "\n"
			"\t mmap size : 0x%" PRIx64 "\n"
			"\t mmap align: 0x%" PRIx64 "\n"
			"\t mmap off  : 0x%" PRIx64 "\n",
			i, reg->size,
			reg->guest_phys_addr,
			reg->guest_user_addr,
			reg->host_user_addr,
			(uint64_t)(uintptr_t)mmap_addr,
			mmap_size,
			alignment,
			mmap_offset);
	}

	dump_guest_pages(dev);

	return 0;

err_mmap:
	free_mem_region(dev);
	rte_free(dev->mem);
	dev->mem = NULL;
	return -1;
}