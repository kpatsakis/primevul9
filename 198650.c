static int cac_select_pki_applet(sc_card_t *card, int index)
{
	sc_path_t applet_path = cac_cac_pki_obj.path;
	applet_path.aid.value[applet_path.aid.len-1] = index;
	return cac_select_file_by_type(card, &applet_path, NULL, SC_CARD_TYPE_CAC_II);
}