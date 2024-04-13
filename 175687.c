static uint64_t intel_vgpu_get_bar_addr(struct intel_vgpu *vgpu, int bar)
{
	u32 start_lo, start_hi;
	u32 mem_type;

	start_lo = (*(u32 *)(vgpu->cfg_space.virtual_cfg_space + bar)) &
			PCI_BASE_ADDRESS_MEM_MASK;
	mem_type = (*(u32 *)(vgpu->cfg_space.virtual_cfg_space + bar)) &
			PCI_BASE_ADDRESS_MEM_TYPE_MASK;

	switch (mem_type) {
	case PCI_BASE_ADDRESS_MEM_TYPE_64:
		start_hi = (*(u32 *)(vgpu->cfg_space.virtual_cfg_space
						+ bar + 4));
		break;
	case PCI_BASE_ADDRESS_MEM_TYPE_32:
	case PCI_BASE_ADDRESS_MEM_TYPE_1M:
		/* 1M mem BAR treated as 32-bit BAR */
	default:
		/* mem unknown type treated as 32-bit BAR */
		start_hi = 0;
		break;
	}

	return ((u64)start_hi << 32) | start_lo;
}