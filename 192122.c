static void mce_schedule_work(void)
{
	if (!mce_gen_pool_empty())
		schedule_work(&mce_work);
}