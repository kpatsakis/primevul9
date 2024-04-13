static void *ptr_from_txbd(struct cdmac_bd *bd)
{
	return (void *)(bd->app4);
}