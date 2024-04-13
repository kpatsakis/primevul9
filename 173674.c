static void generate(void)
{
	int old_lvl, voice;
	struct VOICE_S *p_voice;

	system_init();
	if (!tsfirst)
		return;				/* no symbol */
	set_bar_num();
	if (!tsfirst)
		return;				/* no more symbol */
	old_lvl = lvlarena(2);
	output_music();
	clrarena(2);				/* clear generation */
	lvlarena(old_lvl);

	/* reset the parser */
	for (p_voice = first_voice; p_voice; p_voice = p_voice->next) {
		voice = p_voice - voice_tb;
		p_voice->sym = p_voice->last_sym = NULL;
		p_voice->time = 0;
		p_voice->have_ly = 0;
		p_voice->staff = cursys->voice[voice].staff;
		p_voice->second = cursys->voice[voice].second;
		p_voice->s_clef->time = 0;
		p_voice->lyric_start = NULL;
	}
	staves_found = 0;		// (for voice compress/dup)
}