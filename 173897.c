static void sony_set_leds(struct sony_sc *sc)
{
	if (!(sc->quirks & BUZZ_CONTROLLER))
		sony_schedule_work(sc, SONY_WORKER_STATE);
	else
		buzz_set_leds(sc);
}