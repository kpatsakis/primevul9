static void storm_memset_spq_addr(struct bnx2x *bp,
				  dma_addr_t mapping, u16 abs_fid)
{
	u32 addr = XSEM_REG_FAST_MEMORY +
			XSTORM_SPQ_PAGE_BASE_OFFSET(abs_fid);

	__storm_memset_dma_mapping(bp, addr, mapping);
}