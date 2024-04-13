qva_to_vva(struct virtio_net *dev, uint64_t qva)
{
	struct rte_vhost_mem_region *reg;
	uint32_t i;

	/* Find the region where the address lives. */
	for (i = 0; i < dev->mem->nregions; i++) {
		reg = &dev->mem->regions[i];

		if (qva >= reg->guest_user_addr &&
		    qva <  reg->guest_user_addr + reg->size) {
			return qva - reg->guest_user_addr +
			       reg->host_user_addr;
		}
	}

	return 0;
}