static inline void timer_base_lock_expiry(struct timer_base *base)
{
	spin_lock(&base->expiry_lock);
}