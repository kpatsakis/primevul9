static __init void timer_base_init_expiry_lock(struct timer_base *base)
{
	spin_lock_init(&base->expiry_lock);
}