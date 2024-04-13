static void wcd9335_codec_hph_lohifi_config(struct snd_soc_component *comp,
					  int event)
{
	if (SND_SOC_DAPM_EVENT_ON(event)) {
		snd_soc_component_update_bits(comp, WCD9335_RX_BIAS_HPH_PA,
					WCD9335_RX_BIAS_HPH_PA_AMP_5_UA_MASK,
					0x06);
		snd_soc_component_update_bits(comp,
					WCD9335_RX_BIAS_HPH_RDACBUFF_CNP2,
					0xF0, 0x40);
		snd_soc_component_update_bits(comp, WCD9335_HPH_CNP_WG_CTL,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_MASK,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_RATIO_1000);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
				WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_MASK,
				WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_ENABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL1,
				WCD9335_HPH_PA_GM3_IB_SCALE_MASK,
				0x0C);
		wcd9335_codec_hph_mode_gain_opt(comp, 0x11);
	}

	if (SND_SOC_DAPM_EVENT_OFF(event)) {
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
			WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_MASK,
			WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_DISABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_CNP_WG_CTL,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_MASK,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_RATIO_500);
		snd_soc_component_write(comp, WCD9335_RX_BIAS_HPH_RDACBUFF_CNP2,
					0x8A);
		snd_soc_component_update_bits(comp, WCD9335_RX_BIAS_HPH_PA,
					WCD9335_RX_BIAS_HPH_PA_AMP_5_UA_MASK,
					0x0A);
	}
}