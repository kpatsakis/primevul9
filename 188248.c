void temac_indirect_out32(struct temac_local *lp, int reg, u32 value)
{
	unsigned long flags;

	spin_lock_irqsave(lp->indirect_lock, flags);
	temac_indirect_out32_locked(lp, reg, value);
	spin_unlock_irqrestore(lp->indirect_lock, flags);
}