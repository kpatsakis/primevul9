static void storm_memset_eq_prod(struct bnx2x *bp, u16 eq_prod,
				 u16 pfid)
{
	u32 addr = BAR_CSTRORM_INTMEM + CSTORM_EVENT_RING_PROD_OFFSET(pfid);
	REG_WR16(bp, addr, eq_prod);
}