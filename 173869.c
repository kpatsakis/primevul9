static inline void sony_cancel_work_sync(struct sony_sc *sc)
{
	unsigned long flags;

	if (sc->hotplug_worker_initialized)
		cancel_work_sync(&sc->hotplug_worker);
	if (sc->state_worker_initialized) {
		spin_lock_irqsave(&sc->lock, flags);
		sc->state_worker_initialized = 0;
		spin_unlock_irqrestore(&sc->lock, flags);
		cancel_work_sync(&sc->state_worker);
	}
}