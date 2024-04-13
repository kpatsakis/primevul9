xps_set_color(xps_document *doc, fz_colorspace *colorspace, float *samples)
{
	int i;
	doc->colorspace = colorspace;
	for (i = 0; i < colorspace->n; i++)
		doc->color[i] = samples[i + 1];
	doc->alpha = samples[0] * doc->opacity[doc->opacity_top];
}