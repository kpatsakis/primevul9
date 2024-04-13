void ovs_lock(void)
{
	mutex_lock(&ovs_mutex);
}