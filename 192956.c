static void usage_free(struct snd_seq_usage *res, int num)
{
	res->cur -= num;
}