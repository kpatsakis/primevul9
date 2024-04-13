static inline void timer_base_unlock_expiry(struct timer_base *base)
{
	spin_unlock(&base->expiry_lock);
}