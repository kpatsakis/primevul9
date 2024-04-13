dvi_document_file_exporter_do_page (EvFileExporter  *exporter,
				    EvRenderContext *rc)
{
       DviDocument *dvi_document = DVI_DOCUMENT(exporter);

       g_string_append_printf (dvi_document->exporter_opts, "%d,", (rc->page->index) + 1);
}