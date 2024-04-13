static inline void unlock_timer(struct k_itimer *timr, unsigned long flags)
{
	spin_unlock_irqrestore(&timr->it_lock, flags);
}