static void ptr_to_txbd(void *p, struct cdmac_bd *bd)
{
	bd->app4 = (u32)p;
}