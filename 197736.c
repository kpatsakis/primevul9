static int uac_mixer_unit_get_channels(struct mixer_build *state,
				       struct uac_mixer_unit_descriptor *desc)
{
	int mu_channels;

	if (desc->bLength < 11)
		return -EINVAL;
	if (!desc->bNrInPins)
		return -EINVAL;

	switch (state->mixer->protocol) {
	case UAC_VERSION_1:
	case UAC_VERSION_2:
	default:
		mu_channels = uac_mixer_unit_bNrChannels(desc);
		break;
	case UAC_VERSION_3:
		mu_channels = get_cluster_channels_v3(state,
				uac3_mixer_unit_wClusterDescrID(desc));
		break;
	}

	if (!mu_channels)
		return -EINVAL;

	return mu_channels;
}