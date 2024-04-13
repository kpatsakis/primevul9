static int parse_audio_selector_unit(struct mixer_build *state, int unitid,
				     void *raw_desc)
{
	struct uac_selector_unit_descriptor *desc = raw_desc;
	unsigned int i, nameid, len;
	int err;
	struct usb_mixer_elem_info *cval;
	struct snd_kcontrol *kctl;
	const struct usbmix_name_map *map;
	char **namelist;

	if (desc->bLength < 5 || !desc->bNrInPins ||
	    desc->bLength < 5 + desc->bNrInPins) {
		usb_audio_err(state->chip,
			"invalid SELECTOR UNIT descriptor %d\n", unitid);
		return -EINVAL;
	}

	for (i = 0; i < desc->bNrInPins; i++) {
		err = parse_audio_unit(state, desc->baSourceID[i]);
		if (err < 0)
			return err;
	}

	if (desc->bNrInPins == 1) /* only one ? nonsense! */
		return 0;

	map = find_map(state->map, unitid, 0);
	if (check_ignored_ctl(map))
		return 0;

	cval = kzalloc(sizeof(*cval), GFP_KERNEL);
	if (!cval)
		return -ENOMEM;
	snd_usb_mixer_elem_init_std(&cval->head, state->mixer, unitid);
	cval->val_type = USB_MIXER_U8;
	cval->channels = 1;
	cval->min = 1;
	cval->max = desc->bNrInPins;
	cval->res = 1;
	cval->initialized = 1;

	switch (state->mixer->protocol) {
	case UAC_VERSION_1:
	default:
		cval->control = 0;
		break;
	case UAC_VERSION_2:
	case UAC_VERSION_3:
		if (desc->bDescriptorSubtype == UAC2_CLOCK_SELECTOR ||
		    desc->bDescriptorSubtype == UAC3_CLOCK_SELECTOR)
			cval->control = UAC2_CX_CLOCK_SELECTOR;
		else /* UAC2/3_SELECTOR_UNIT */
			cval->control = UAC2_SU_SELECTOR;
		break;
	}

	namelist = kmalloc_array(desc->bNrInPins, sizeof(char *), GFP_KERNEL);
	if (!namelist) {
		kfree(cval);
		return -ENOMEM;
	}
#define MAX_ITEM_NAME_LEN	64
	for (i = 0; i < desc->bNrInPins; i++) {
		struct usb_audio_term iterm;
		len = 0;
		namelist[i] = kmalloc(MAX_ITEM_NAME_LEN, GFP_KERNEL);
		if (!namelist[i]) {
			while (i--)
				kfree(namelist[i]);
			kfree(namelist);
			kfree(cval);
			return -ENOMEM;
		}
		len = check_mapped_selector_name(state, unitid, i, namelist[i],
						 MAX_ITEM_NAME_LEN);
		if (! len && check_input_term(state, desc->baSourceID[i], &iterm) >= 0)
			len = get_term_name(state->chip, &iterm, namelist[i],
					    MAX_ITEM_NAME_LEN, 0);
		if (! len)
			sprintf(namelist[i], "Input %u", i);
	}

	kctl = snd_ctl_new1(&mixer_selectunit_ctl, cval);
	if (! kctl) {
		usb_audio_err(state->chip, "cannot malloc kcontrol\n");
		kfree(namelist);
		kfree(cval);
		return -ENOMEM;
	}
	kctl->private_value = (unsigned long)namelist;
	kctl->private_free = usb_mixer_selector_elem_free;

	/* check the static mapping table at first */
	len = check_mapped_name(map, kctl->id.name, sizeof(kctl->id.name));
	if (!len) {
		/* no mapping ? */
		switch (state->mixer->protocol) {
		case UAC_VERSION_1:
		case UAC_VERSION_2:
		default:
		/* if iSelector is given, use it */
			nameid = uac_selector_unit_iSelector(desc);
			if (nameid)
				len = snd_usb_copy_string_desc(state->chip,
							nameid, kctl->id.name,
							sizeof(kctl->id.name));
			break;
		case UAC_VERSION_3:
			/* TODO: Class-Specific strings not yet supported */
			break;
		}

		/* ... or pick up the terminal name at next */
		if (!len)
			len = get_term_name(state->chip, &state->oterm,
				    kctl->id.name, sizeof(kctl->id.name), 0);
		/* ... or use the fixed string "USB" as the last resort */
		if (!len)
			strlcpy(kctl->id.name, "USB", sizeof(kctl->id.name));

		/* and add the proper suffix */
		if (desc->bDescriptorSubtype == UAC2_CLOCK_SELECTOR ||
		    desc->bDescriptorSubtype == UAC3_CLOCK_SELECTOR)
			append_ctl_name(kctl, " Clock Source");
		else if ((state->oterm.type & 0xff00) == 0x0100)
			append_ctl_name(kctl, " Capture Source");
		else
			append_ctl_name(kctl, " Playback Source");
	}

	usb_audio_dbg(state->chip, "[%d] SU [%s] items = %d\n",
		    cval->head.id, kctl->id.name, desc->bNrInPins);
	return snd_usb_mixer_add_control(&cval->head, kctl);
}