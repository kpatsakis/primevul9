static int check_event_type_and_length(struct snd_seq_event *ev)
{
	switch (snd_seq_ev_length_type(ev)) {
	case SNDRV_SEQ_EVENT_LENGTH_FIXED:
		if (snd_seq_ev_is_variable_type(ev))
			return -EINVAL;
		break;
	case SNDRV_SEQ_EVENT_LENGTH_VARIABLE:
		if (! snd_seq_ev_is_variable_type(ev) ||
		    (ev->data.ext.len & ~SNDRV_SEQ_EXT_MASK) >= SNDRV_SEQ_MAX_EVENT_LEN)
			return -EINVAL;
		break;
	case SNDRV_SEQ_EVENT_LENGTH_VARUSR:
		if (! snd_seq_ev_is_direct(ev))
			return -EINVAL;
		break;
	}
	return 0;
}