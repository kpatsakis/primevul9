static void ptr_to_txbd(void *p, struct cdmac_bd *bd)
{
	bd->app3 = (u32)(((u64)p) >> 32);
	bd->app4 = (u32)((u64)p & 0xFFFFFFFF);
}