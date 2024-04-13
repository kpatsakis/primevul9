static void wcd9335_codec_hph_lp_config(struct snd_soc_component *comp,
				      int event)
{
	if (SND_SOC_DAPM_EVENT_ON(event)) {
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL1,
				WCD9335_HPH_PA_GM3_IB_SCALE_MASK,
				0x0C);
		wcd9335_codec_hph_mode_gain_opt(comp, 0x10);
		snd_soc_component_update_bits(comp, WCD9335_HPH_CNP_WG_CTL,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_MASK,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_RATIO_1000);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
			WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_MASK,
			WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_ENABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
				WCD9335_HPH_PA_CTL2_FORCE_PSRREH_MASK,
				WCD9335_HPH_PA_CTL2_FORCE_PSRREH_ENABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
				WCD9335_HPH_PA_CTL2_HPH_PSRR_ENH_MASK,
				WCD9335_HPH_PA_CTL2_HPH_PSRR_ENABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_RDAC_LDO_CTL,
				WCD9335_HPH_RDAC_N1P65_LD_OUTCTL_MASK,
				WCD9335_HPH_RDAC_N1P65_LD_OUTCTL_V_N1P60);
		snd_soc_component_update_bits(comp, WCD9335_HPH_RDAC_LDO_CTL,
				WCD9335_HPH_RDAC_1P65_LD_OUTCTL_MASK,
				WCD9335_HPH_RDAC_1P65_LD_OUTCTL_V_N1P60);
		snd_soc_component_update_bits(comp,
				WCD9335_RX_BIAS_HPH_RDAC_LDO, 0x0F, 0x01);
		snd_soc_component_update_bits(comp,
				WCD9335_RX_BIAS_HPH_RDAC_LDO, 0xF0, 0x10);
	}

	if (SND_SOC_DAPM_EVENT_OFF(event)) {
		snd_soc_component_write(comp, WCD9335_RX_BIAS_HPH_RDAC_LDO,
					0x88);
		snd_soc_component_write(comp, WCD9335_HPH_RDAC_LDO_CTL,
					0x33);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
				WCD9335_HPH_PA_CTL2_HPH_PSRR_ENH_MASK,
				WCD9335_HPH_PA_CTL2_HPH_PSRR_DISABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
				WCD9335_HPH_PA_CTL2_FORCE_PSRREH_MASK,
				WCD9335_HPH_PA_CTL2_FORCE_PSRREH_DISABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_PA_CTL2,
				WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_MASK,
				WCD9335_HPH_PA_CTL2_FORCE_IQCTRL_DISABLE);
		snd_soc_component_update_bits(comp, WCD9335_HPH_CNP_WG_CTL,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_MASK,
				WCD9335_HPH_CNP_WG_CTL_CURR_LDIV_RATIO_500);
		snd_soc_component_update_bits(comp, WCD9335_HPH_R_EN,
				WCD9335_HPH_CONST_SEL_L_MASK,
				WCD9335_HPH_CONST_SEL_L_HQ_PATH);
		snd_soc_component_update_bits(comp, WCD9335_HPH_L_EN,
				WCD9335_HPH_CONST_SEL_L_MASK,
				WCD9335_HPH_CONST_SEL_L_HQ_PATH);
	}
}