static int parse_audio_feature_unit(struct mixer_build *state, int unitid,
				    void *_ftr)
{
	int channels, i, j;
	struct usb_audio_term iterm;
	unsigned int master_bits, first_ch_bits;
	int err, csize;
	struct uac_feature_unit_descriptor *hdr = _ftr;
	__u8 *bmaControls;

	if (state->mixer->protocol == UAC_VERSION_1) {
		if (hdr->bLength < 7) {
			usb_audio_err(state->chip,
				      "unit %u: invalid UAC_FEATURE_UNIT descriptor\n",
				      unitid);
			return -EINVAL;
		}
		csize = hdr->bControlSize;
		if (!csize) {
			usb_audio_dbg(state->chip,
				      "unit %u: invalid bControlSize == 0\n",
				      unitid);
			return -EINVAL;
		}
		channels = (hdr->bLength - 7) / csize - 1;
		bmaControls = hdr->bmaControls;
		if (hdr->bLength < 7 + csize) {
			usb_audio_err(state->chip,
				      "unit %u: invalid UAC_FEATURE_UNIT descriptor\n",
				      unitid);
			return -EINVAL;
		}
	} else if (state->mixer->protocol == UAC_VERSION_2) {
		struct uac2_feature_unit_descriptor *ftr = _ftr;
		if (hdr->bLength < 6) {
			usb_audio_err(state->chip,
				      "unit %u: invalid UAC_FEATURE_UNIT descriptor\n",
				      unitid);
			return -EINVAL;
		}
		csize = 4;
		channels = (hdr->bLength - 6) / 4 - 1;
		bmaControls = ftr->bmaControls;
		if (hdr->bLength < 6 + csize) {
			usb_audio_err(state->chip,
				      "unit %u: invalid UAC_FEATURE_UNIT descriptor\n",
				      unitid);
			return -EINVAL;
		}
	} else { /* UAC_VERSION_3 */
		struct uac3_feature_unit_descriptor *ftr = _ftr;

		if (hdr->bLength < 7) {
			usb_audio_err(state->chip,
				      "unit %u: invalid UAC3_FEATURE_UNIT descriptor\n",
				      unitid);
			return -EINVAL;
		}
		csize = 4;
		channels = (ftr->bLength - 7) / 4 - 1;
		bmaControls = ftr->bmaControls;
		if (hdr->bLength < 7 + csize) {
			usb_audio_err(state->chip,
				      "unit %u: invalid UAC3_FEATURE_UNIT descriptor\n",
				      unitid);
			return -EINVAL;
		}
	}

	/* parse the source unit */
	err = parse_audio_unit(state, hdr->bSourceID);
	if (err < 0)
		return err;

	/* determine the input source type and name */
	err = check_input_term(state, hdr->bSourceID, &iterm);
	if (err < 0)
		return err;

	master_bits = snd_usb_combine_bytes(bmaControls, csize);
	/* master configuration quirks */
	switch (state->chip->usb_id) {
	case USB_ID(0x08bb, 0x2702):
		usb_audio_info(state->chip,
			       "usbmixer: master volume quirk for PCM2702 chip\n");
		/* disable non-functional volume control */
		master_bits &= ~UAC_CONTROL_BIT(UAC_FU_VOLUME);
		break;
	case USB_ID(0x1130, 0xf211):
		usb_audio_info(state->chip,
			       "usbmixer: volume control quirk for Tenx TP6911 Audio Headset\n");
		/* disable non-functional volume control */
		channels = 0;
		break;

	}
	if (channels > 0)
		first_ch_bits = snd_usb_combine_bytes(bmaControls + csize, csize);
	else
		first_ch_bits = 0;

	if (state->mixer->protocol == UAC_VERSION_1) {
		/* check all control types */
		for (i = 0; i < 10; i++) {
			unsigned int ch_bits = 0;
			int control = audio_feature_info[i].control;

			for (j = 0; j < channels; j++) {
				unsigned int mask;

				mask = snd_usb_combine_bytes(bmaControls +
							     csize * (j+1), csize);
				if (mask & (1 << i))
					ch_bits |= (1 << j);
			}
			/* audio class v1 controls are never read-only */

			/*
			 * The first channel must be set
			 * (for ease of programming).
			 */
			if (ch_bits & 1)
				build_feature_ctl(state, _ftr, ch_bits, control,
						  &iterm, unitid, 0);
			if (master_bits & (1 << i))
				build_feature_ctl(state, _ftr, 0, control,
						  &iterm, unitid, 0);
		}
	} else { /* UAC_VERSION_2/3 */
		for (i = 0; i < ARRAY_SIZE(audio_feature_info); i++) {
			unsigned int ch_bits = 0;
			unsigned int ch_read_only = 0;
			int control = audio_feature_info[i].control;

			for (j = 0; j < channels; j++) {
				unsigned int mask;

				mask = snd_usb_combine_bytes(bmaControls +
							     csize * (j+1), csize);
				if (uac_v2v3_control_is_readable(mask, control)) {
					ch_bits |= (1 << j);
					if (!uac_v2v3_control_is_writeable(mask, control))
						ch_read_only |= (1 << j);
				}
			}

			/*
			 * NOTE: build_feature_ctl() will mark the control
			 * read-only if all channels are marked read-only in
			 * the descriptors. Otherwise, the control will be
			 * reported as writeable, but the driver will not
			 * actually issue a write command for read-only
			 * channels.
			 */

			/*
			 * The first channel must be set
			 * (for ease of programming).
			 */
			if (ch_bits & 1)
				build_feature_ctl(state, _ftr, ch_bits, control,
						  &iterm, unitid, ch_read_only);
			if (uac_v2v3_control_is_readable(master_bits, control))
				build_feature_ctl(state, _ftr, 0, control,
						  &iterm, unitid,
						  !uac_v2v3_control_is_writeable(master_bits,
										 control));
		}
	}

	return 0;
}