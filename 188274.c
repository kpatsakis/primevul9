static void *ptr_from_txbd(struct cdmac_bd *bd)
{
	return (void *)(((u64)(bd->app3) << 32) | bd->app4);
}