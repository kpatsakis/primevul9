void mce_inject_log(struct mce *m)
{
	mutex_lock(&mce_log_mutex);
	mce_log(m);
	mutex_unlock(&mce_log_mutex);
}