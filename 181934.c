static int slim_tx_mixer_put(struct snd_kcontrol *kc,
			     struct snd_ctl_elem_value *ucontrol)
{

	struct snd_soc_dapm_widget *widget = snd_soc_dapm_kcontrol_widget(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(widget->dapm->dev);
	struct snd_soc_dapm_update *update = NULL;
	struct soc_mixer_control *mixer =
			(struct soc_mixer_control *)kc->private_value;
	int enable = ucontrol->value.integer.value[0];
	int dai_id = widget->shift;
	int port_id = mixer->shift;

	switch (dai_id) {
	case AIF1_CAP:
	case AIF2_CAP:
	case AIF3_CAP:
		/* only add to the list if value not set */
		if (enable && !(wcd->tx_port_value & BIT(port_id))) {
			wcd->tx_port_value |= BIT(port_id);
			list_add_tail(&wcd->tx_chs[port_id].list,
					&wcd->dai[dai_id].slim_ch_list);
		} else if (!enable && (wcd->tx_port_value & BIT(port_id))) {
			wcd->tx_port_value &= ~BIT(port_id);
			list_del_init(&wcd->tx_chs[port_id].list);
		}
		break;
	default:
		dev_err(wcd->dev, "Unknown AIF %d\n", dai_id);
		return -EINVAL;
	}

	snd_soc_dapm_mixer_update_power(widget->dapm, kc, enable, update);

	return 0;
}