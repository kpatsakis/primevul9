static inline void sony_send_output_report(struct sony_sc *sc)
{
	if (sc->send_output_report)
		sc->send_output_report(sc);
}