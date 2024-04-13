static inline void sony_schedule_work(struct sony_sc *sc,
				      enum sony_worker which)
{
	unsigned long flags;

	switch (which) {
	case SONY_WORKER_STATE:
		spin_lock_irqsave(&sc->lock, flags);
		if (!sc->defer_initialization && sc->state_worker_initialized)
			schedule_work(&sc->state_worker);
		spin_unlock_irqrestore(&sc->lock, flags);
		break;
	case SONY_WORKER_HOTPLUG:
		if (sc->hotplug_worker_initialized)
			schedule_work(&sc->hotplug_worker);
		break;
	}
}