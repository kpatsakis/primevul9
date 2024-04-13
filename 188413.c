xps_draw_one_radial_gradient(xps_document *doc, const fz_matrix *ctm,
	struct stop *stops, int count,
	int extend,
	float x0, float y0, float r0,
	float x1, float y1, float r1)
{
	fz_shade *shade;

	/* TODO: this (and the stuff in pdf_shade) should move to res_shade.c */
	shade = fz_malloc_struct(doc->ctx, fz_shade);
	FZ_INIT_STORABLE(shade, 1, fz_free_shade_imp);
	shade->colorspace = fz_device_rgb(doc->ctx);
	shade->bbox = fz_infinite_rect;
	shade->matrix = fz_identity;
	shade->use_background = 0;
	shade->use_function = 1;
	shade->type = FZ_RADIAL;
	shade->u.l_or_r.extend[0] = extend;
	shade->u.l_or_r.extend[1] = extend;

	xps_sample_gradient_stops(shade, stops, count);

	shade->u.l_or_r.coords[0][0] = x0;
	shade->u.l_or_r.coords[0][1] = y0;
	shade->u.l_or_r.coords[0][2] = r0;
	shade->u.l_or_r.coords[1][0] = x1;
	shade->u.l_or_r.coords[1][1] = y1;
	shade->u.l_or_r.coords[1][2] = r1;

	fz_fill_shade(doc->dev, shade, ctm, 1);

	fz_drop_shade(doc->ctx, shade);
}