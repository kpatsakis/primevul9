static void voice_init(void)
{
	struct VOICE_S *p_voice;
	int i;

	for (i = 0, p_voice = voice_tb;
	     i < MAXVOICE;
	     i++, p_voice++) {
		p_voice->sym = p_voice->last_sym = NULL;
		p_voice->lyric_start = NULL;
		p_voice->bar_start = 0;
		p_voice->time = 0;
		p_voice->slur_st = 0;
		p_voice->hy_st = 0;
		p_voice->tie = 0;
		p_voice->rtie = 0;
	}
}