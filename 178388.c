dvi_document_file_exporter_iface_init (EvFileExporterInterface *iface)
{
        iface->begin = dvi_document_file_exporter_begin;
        iface->do_page = dvi_document_file_exporter_do_page;
        iface->end = dvi_document_file_exporter_end;
	iface->get_capabilities = dvi_document_file_exporter_get_capabilities;
}