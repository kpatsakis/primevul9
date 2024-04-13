void setup_per_zone_wmarks(void)
{
	static DEFINE_SPINLOCK(lock);

	spin_lock(&lock);
	__setup_per_zone_wmarks();
	spin_unlock(&lock);
}