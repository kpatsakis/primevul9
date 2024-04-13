static void entropy_timer(struct timer_list *t)
{
	credit_entropy_bits(&input_pool, 1);
}