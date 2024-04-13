static void wcd9335_codec_set_tx_hold(struct snd_soc_component *comp,
				      u16 amic_reg, bool set)
{
	u8 mask = 0x20;
	u8 val;

	if (amic_reg == WCD9335_ANA_AMIC1 || amic_reg == WCD9335_ANA_AMIC3 ||
	    amic_reg == WCD9335_ANA_AMIC5)
		mask = 0x40;

	val = set ? mask : 0x00;

	switch (amic_reg) {
	case WCD9335_ANA_AMIC1:
	case WCD9335_ANA_AMIC2:
		snd_soc_component_update_bits(comp, WCD9335_ANA_AMIC2, mask,
						val);
		break;
	case WCD9335_ANA_AMIC3:
	case WCD9335_ANA_AMIC4:
		snd_soc_component_update_bits(comp, WCD9335_ANA_AMIC4, mask,
						val);
		break;
	case WCD9335_ANA_AMIC5:
	case WCD9335_ANA_AMIC6:
		snd_soc_component_update_bits(comp, WCD9335_ANA_AMIC6, mask,
						val);
		break;
	default:
		dev_err(comp->dev, "%s: invalid amic: %d\n",
			__func__, amic_reg);
		break;
	}
}