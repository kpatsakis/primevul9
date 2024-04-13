static bool check_user_elem_overflow(struct snd_card *card, ssize_t add)
{
	return (ssize_t)card->user_ctl_alloc_size + add > max_user_ctl_alloc_size;
}