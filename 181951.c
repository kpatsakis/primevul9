static void wcd9335_codec_init_flyback(struct snd_soc_component *component)
{
	snd_soc_component_update_bits(component, WCD9335_HPH_L_EN,
					WCD9335_HPH_CONST_SEL_L_MASK,
					WCD9335_HPH_CONST_SEL_L_BYPASS);
	snd_soc_component_update_bits(component, WCD9335_HPH_R_EN,
					WCD9335_HPH_CONST_SEL_L_MASK,
					WCD9335_HPH_CONST_SEL_L_BYPASS);
	snd_soc_component_update_bits(component, WCD9335_RX_BIAS_FLYB_BUFF,
					WCD9335_RX_BIAS_FLYB_VPOS_5_UA_MASK,
					WCD9335_RX_BIAS_FLYB_I_0P0_UA);
	snd_soc_component_update_bits(component, WCD9335_RX_BIAS_FLYB_BUFF,
					WCD9335_RX_BIAS_FLYB_VNEG_5_UA_MASK,
					WCD9335_RX_BIAS_FLYB_I_0P0_UA);
}