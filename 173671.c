static void get_voice(struct SYMBOL *s)
{
	struct VOICE_S *p_voice;
	int voice;

	voice = s->u.voice.voice;
	p_voice = &voice_tb[voice];
	if (parsys->voice[voice].range < 0) {
		if (cfmt.alignbars) {
			error(1, s, "V: does not work with %%%%alignbars");
		}
		if (staves_found < 0) {
			if (!s->u.voice.merge) {
#if MAXSTAFF < MAXVOICE
				if (nstaff >= MAXSTAFF - 1) {
					error(1, s, "Too many staves");
					return;
				}
#endif
				nstaff++;
			} else {
				p_voice->second = 1;
				parsys->voice[voice].second = 1;
			}
			p_voice->staff = p_voice->cstaff = nstaff;
			parsys->voice[voice].staff = nstaff;
			parsys->nstaff = nstaff;
			{
				int range, i;

				range = 0;
				for (i = 0; i < MAXVOICE; i++) {
					if (parsys->voice[i].range > range)
						range = parsys->voice[i].range;
				}
				parsys->voice[voice].range = range + 1;
				voice_link(p_voice);
			}
		} else {
			p_voice->ignore = 1;
			p_voice->staff = p_voice->cstaff = nstaff + 1;
		}
	}

	/* if something has changed, update */
	if (s->u.voice.fname != 0) {
		p_voice->nm = s->u.voice.fname;
		p_voice->new_name = 1;
	}
	if (s->u.voice.nname != 0)
		p_voice->snm = s->u.voice.nname;
	if (s->u.voice.octave != NO_OCTAVE)
		p_voice->octave = s->u.voice.octave;
	switch (s->u.voice.dyn) {
	case 1:
		p_voice->posit.dyn = SL_ABOVE;
		p_voice->posit.vol = SL_ABOVE;
		break;
	case -1:
		p_voice->posit.dyn = SL_BELOW;
		p_voice->posit.vol = SL_BELOW;
		break;
	}
	switch (s->u.voice.lyrics) {
	case 1:
		p_voice->posit.voc = SL_ABOVE;
		break;
	case -1:
		p_voice->posit.voc = SL_BELOW;
		break;
	}
	switch (s->u.voice.gchord) {
	case 1:
		p_voice->posit.gch = SL_ABOVE;
		break;
	case -1:
		p_voice->posit.gch = SL_BELOW;
		break;
	}
	switch (s->u.voice.stem) {
	case 1:
		p_voice->posit.std = SL_ABOVE;
		break;
	case -1:
		p_voice->posit.std = SL_BELOW;
		break;
	case 2:
		p_voice->posit.std = 0;		/* auto */
		break;
	}
	switch (s->u.voice.gstem) {
	case 1:
		p_voice->posit.gsd = SL_ABOVE;
		break;
	case -1:
		p_voice->posit.gsd = SL_BELOW;
		break;
	case 2:
		p_voice->posit.gsd = 0;		/* auto */
		break;
	}
	if (s->u.voice.scale != 0)
		p_voice->scale = s->u.voice.scale;
	else if (s->u.voice.cue > 0)
		p_voice->scale = 0.7;
	else if (s->u.voice.cue < 0)
		p_voice->scale = 1;
	if (s->u.voice.stafflines)
		p_voice->stafflines = s->u.voice.stafflines;
	if (s->u.voice.staffscale != 0)
		p_voice->staffscale = s->u.voice.staffscale;
	if (!p_voice->combine)
		p_voice->combine = cfmt.combinevoices;

	set_tblt(p_voice);

	/* if in tune, switch to this voice */
	if (s->state == ABC_S_TUNE)
		curvoice = p_voice;
}