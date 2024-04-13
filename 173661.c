static void system_new(void)
{
	struct SYSTEM *new_sy;
	int staff, voice;

	new_sy = (struct SYSTEM *) getarena(sizeof *new_sy);
	if (!parsys) {
		memset(new_sy, 0, sizeof *new_sy);
		for (voice = 0; voice < MAXVOICE; voice++) {
			new_sy->voice[voice].range = -1;
		}
		for (staff = 0; staff < MAXSTAFF; staff++) {
			new_sy->staff[staff].stafflines = "|||||";
			new_sy->staff[staff].staffscale = 1;
		}
		cursys = new_sy;
	} else {
		for (voice = 0; voice < MAXVOICE; voice++) {

			// update the previous system
//			if (parsys->voice[voice].range < 0
//			 || parsys->voice[voice].second)
//				continue;
			staff = parsys->voice[voice].staff;
			if (voice_tb[voice].stafflines)
				parsys->staff[staff].stafflines =
						voice_tb[voice].stafflines;
			if (voice_tb[voice].staffscale != 0)
				parsys->staff[staff].staffscale =
						voice_tb[voice].staffscale;
		}
		memcpy(new_sy, parsys, sizeof *new_sy);
		
		for (voice = 0; voice < MAXVOICE; voice++) {
			new_sy->voice[voice].range = -1;
			new_sy->voice[voice].second = 0;
		}
		for (staff = 0; staff < MAXSTAFF; staff++)
			new_sy->staff[staff].flags = 0;
		parsys->next = new_sy;
	}
	parsys = new_sy;
}