static int fanout_rr_next(struct packet_fanout *f, unsigned int num)
{
	int x = atomic_read(&f->rr_cur) + 1;

	if (x >= num)
		x = 0;

	return x;
}