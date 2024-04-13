dvi_document_file_exporter_begin (EvFileExporter        *exporter,
				  EvFileExporterContext *fc)
{
	DviDocument *dvi_document = DVI_DOCUMENT(exporter);
	
	if (dvi_document->exporter_filename)
		g_free (dvi_document->exporter_filename);	
	dvi_document->exporter_filename = g_strdup (fc->filename);
	
	if (dvi_document->exporter_opts) {
		g_string_free (dvi_document->exporter_opts, TRUE);
	}
	dvi_document->exporter_opts = g_string_new ("-s ");
}