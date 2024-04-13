static inline void sony_init_output_report(struct sony_sc *sc,
				void (*send_output_report)(struct sony_sc *))
{
	sc->send_output_report = send_output_report;

	if (!sc->state_worker_initialized)
		INIT_WORK(&sc->state_worker, sony_state_worker);

	sc->state_worker_initialized = 1;
}