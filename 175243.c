static int nfq_id_after(unsigned int id, unsigned int max)
{
	return (int)(id - max) > 0;
}