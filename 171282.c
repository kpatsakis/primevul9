vhost_user_set_log_base(struct virtio_net *dev, struct VhostUserMsg *msg)
{
	int fd = msg->fds[0];
	uint64_t size, off;
	void *addr;

	if (fd < 0) {
		RTE_LOG(ERR, VHOST_CONFIG, "invalid log fd: %d\n", fd);
		return -1;
	}

	if (msg->size != sizeof(VhostUserLog)) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"invalid log base msg size: %"PRId32" != %d\n",
			msg->size, (int)sizeof(VhostUserLog));
		return -1;
	}

	size = msg->payload.log.mmap_size;
	off  = msg->payload.log.mmap_offset;

	/* Don't allow mmap_offset to point outside the mmap region */
	if (off > size) {
		RTE_LOG(ERR, VHOST_CONFIG,
			"log offset %#"PRIx64" exceeds log size %#"PRIx64"\n",
			off, size);
		return -1;
	}

	RTE_LOG(INFO, VHOST_CONFIG,
		"log mmap size: %"PRId64", offset: %"PRId64"\n",
		size, off);

	/*
	 * mmap from 0 to workaround a hugepage mmap bug: mmap will
	 * fail when offset is not page size aligned.
	 */
	addr = mmap(0, size + off, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);
	if (addr == MAP_FAILED) {
		RTE_LOG(ERR, VHOST_CONFIG, "mmap log base failed!\n");
		return -1;
	}

	/*
	 * Free previously mapped log memory on occasionally
	 * multiple VHOST_USER_SET_LOG_BASE.
	 */
	if (dev->log_addr) {
		munmap((void *)(uintptr_t)dev->log_addr, dev->log_size);
	}
	dev->log_addr = (uint64_t)(uintptr_t)addr;
	dev->log_base = dev->log_addr + off;
	dev->log_size = size;

	return 0;
}