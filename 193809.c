static int select_pkcs15_app(sc_card_t * card)
{
	sc_path_t app;
	int r;

	/* Regular PKCS#15 AID */
	sc_format_path("A000000063504B43532D3135", &app);
	app.type = SC_PATH_TYPE_DF_NAME;
	r = sc_select_file(card, &app, NULL);
	return r;
}