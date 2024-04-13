static void voice_link(struct VOICE_S *p_voice)
{
	struct VOICE_S *p_voice2;

	p_voice2 = first_voice;
	for (;;) {
		if (p_voice2 == p_voice)
			return;
		if (!p_voice2->next)
			break;
		p_voice2 = p_voice2->next;
	}
	p_voice2->next = p_voice;
}