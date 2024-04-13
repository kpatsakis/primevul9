dvi_document_save (EvDocument  *document,
		      const char  *uri,
		      GError     **error)
{
	DviDocument *dvi_document = DVI_DOCUMENT (document);

	return ev_xfer_uri_simple (dvi_document->uri, uri, error);
}