dvi_document_init_params (DviDocument *dvi_document)
{	
	dvi_document->params = g_new0 (DviParams, 1);	

	dvi_document->params->dpi      = MDVI_DPI;
	dvi_document->params->vdpi     = MDVI_VDPI;
	dvi_document->params->mag      = MDVI_MAGNIFICATION;
	dvi_document->params->density  = MDVI_DEFAULT_DENSITY;
	dvi_document->params->gamma    = MDVI_DEFAULT_GAMMA;
	dvi_document->params->flags    = MDVI_PARAM_ANTIALIASED;
	dvi_document->params->hdrift   = 0;
	dvi_document->params->vdrift   = 0;
	dvi_document->params->hshrink  =  MDVI_SHRINK_FROM_DPI(dvi_document->params->dpi);
	dvi_document->params->vshrink  =  MDVI_SHRINK_FROM_DPI(dvi_document->params->vdpi);
	dvi_document->params->orientation = MDVI_ORIENT_TBLR;

	dvi_document->spec = NULL;
	
        dvi_document->params->bg = 0xffffffff;
        dvi_document->params->fg = 0xff000000;
}