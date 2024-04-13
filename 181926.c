static int slim_rx_mux_put(struct snd_kcontrol *kc,
			   struct snd_ctl_elem_value *ucontrol)
{
	struct snd_soc_dapm_widget *w = snd_soc_dapm_kcontrol_widget(kc);
	struct wcd9335_codec *wcd = dev_get_drvdata(w->dapm->dev);
	struct soc_enum *e = (struct soc_enum *)kc->private_value;
	struct snd_soc_dapm_update *update = NULL;
	u32 port_id = w->shift;

	wcd->rx_port_value = ucontrol->value.enumerated.item[0];

	switch (wcd->rx_port_value) {
	case 0:
		list_del_init(&wcd->rx_chs[port_id].list);
		break;
	case 1:
		list_add_tail(&wcd->rx_chs[port_id].list,
			      &wcd->dai[AIF1_PB].slim_ch_list);
		break;
	case 2:
		list_add_tail(&wcd->rx_chs[port_id].list,
			      &wcd->dai[AIF2_PB].slim_ch_list);
		break;
	case 3:
		list_add_tail(&wcd->rx_chs[port_id].list,
			      &wcd->dai[AIF3_PB].slim_ch_list);
		break;
	case 4:
		list_add_tail(&wcd->rx_chs[port_id].list,
			      &wcd->dai[AIF4_PB].slim_ch_list);
		break;
	default:
		dev_err(wcd->dev, "Unknown AIF %d\n", wcd->rx_port_value);
		goto err;
	}

	snd_soc_dapm_mux_update_power(w->dapm, kc, wcd->rx_port_value,
				      e, update);

	return 0;
err:
	return -EINVAL;
}