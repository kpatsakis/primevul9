static void set_tblt(struct VOICE_S *p_voice)
{
	struct tblt_s *tblt;
	int i;

	for (i = 0; i < ncmdtblt; i++) {
		if (!cmdtblts[i].active)
			continue;
		if (cmdtblts[i].vn[0] != '\0') {
			if (strcmp(cmdtblts[i].vn, p_voice->id) != 0
			 && (p_voice->nm == 0
			  || strcmp(cmdtblts[i].vn, p_voice->nm) != 0)
			 && (p_voice->snm == 0
			  || strcmp(cmdtblts[i].vn, p_voice->snm) != 0))
				continue;
		}
		tblt = tblts[cmdtblts[i].index];
		if (p_voice->tblts[0] == tblt
		 || p_voice->tblts[1] == tblt)
			continue;
		if (p_voice->tblts[0] == 0)
			p_voice->tblts[0] = tblt;
		else
			p_voice->tblts[1] = tblt;
	}
}