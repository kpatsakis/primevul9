static void __storm_memset_dma_mapping(struct bnx2x *bp,
				       u32 addr, dma_addr_t mapping)
{
	REG_WR(bp,  addr, U64_LO(mapping));
	REG_WR(bp,  addr + 4, U64_HI(mapping));
}