dvi_document_class_init (DviDocumentClass *klass)
{
	GObjectClass    *gobject_class = G_OBJECT_CLASS (klass);
	EvDocumentClass *ev_document_class = EV_DOCUMENT_CLASS (klass);
	gchar *texmfcnf;

	gobject_class->finalize = dvi_document_finalize;

	texmfcnf = get_texmfcnf();
	mdvi_init_kpathsea ("evince", MDVI_MFMODE, MDVI_FALLBACK_FONT, MDVI_DPI, texmfcnf);
	g_free(texmfcnf);

	mdvi_register_special ("Color", "color", NULL, dvi_document_do_color_special, 1);
	mdvi_register_fonts ();

	ev_document_class->load = dvi_document_load;
	ev_document_class->save = dvi_document_save;
	ev_document_class->get_n_pages = dvi_document_get_n_pages;
	ev_document_class->get_page_size = dvi_document_get_page_size;
	ev_document_class->render = dvi_document_render;
	ev_document_class->support_synctex = dvi_document_support_synctex;
}