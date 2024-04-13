dvi_document_get_page_size (EvDocument *document,
			    EvPage     *page,
			    double     *width,
			    double     *height)
{
	DviDocument *dvi_document = DVI_DOCUMENT (document);	

        *width = dvi_document->base_width;
        *height = dvi_document->base_height;;
}