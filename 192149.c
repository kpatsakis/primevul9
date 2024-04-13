static void print_mce(struct mce *m)
{
	__print_mce(m);
	pr_emerg_ratelimited(HW_ERR "Run the above through 'mcelog --ascii'\n");
}