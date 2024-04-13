static u32 temac_setoptions(struct net_device *ndev, u32 options)
{
	struct temac_local *lp = netdev_priv(ndev);
	struct temac_option *tp = &temac_options[0];
	int reg;
	unsigned long flags;

	spin_lock_irqsave(lp->indirect_lock, flags);
	while (tp->opt) {
		reg = temac_indirect_in32_locked(lp, tp->reg) & ~tp->m_or;
		if (options & tp->opt) {
			reg |= tp->m_or;
			temac_indirect_out32_locked(lp, tp->reg, reg);
		}
		tp++;
	}
	spin_unlock_irqrestore(lp->indirect_lock, flags);
	lp->options |= options;

	return 0;
}