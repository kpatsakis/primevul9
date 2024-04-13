static void sony_state_worker(struct work_struct *work)
{
	struct sony_sc *sc = container_of(work, struct sony_sc, state_worker);

	sc->send_output_report(sc);
}