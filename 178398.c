dvi_document_init (DviDocument *dvi_document)
{
	dvi_document->context = NULL;
	dvi_document_init_params (dvi_document);

	dvi_document->exporter_filename = NULL;
	dvi_document->exporter_opts = NULL;
}