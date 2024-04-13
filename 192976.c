static void usage_alloc(struct snd_seq_usage *res, int num)
{
	res->cur += num;
	if (res->cur > res->peak)
		res->peak = res->cur;
}