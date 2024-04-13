u32 temac_indirect_in32(struct temac_local *lp, int reg)
{
	unsigned long flags;
	int val;

	spin_lock_irqsave(lp->indirect_lock, flags);
	val = temac_indirect_in32_locked(lp, reg);
	spin_unlock_irqrestore(lp->indirect_lock, flags);
	return val;
}