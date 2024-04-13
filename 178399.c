dvi_document_get_n_pages (EvDocument *document)
{
	DviDocument *dvi_document = DVI_DOCUMENT (document);
	
	return dvi_document->context->npages;
}