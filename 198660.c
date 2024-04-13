static int muscle_card_verified_pins(sc_card_t *card, sc_cardctl_muscle_verified_pins_info_t *info)
{
	muscle_private_t* priv = MUSCLE_DATA(card);
	info->verifiedPins = priv->verifiedPins;
	return 0;
}