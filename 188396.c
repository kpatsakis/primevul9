xps_draw_arc_segment(fz_context *doc, fz_path *path, const fz_matrix *mtx, float th0, float th1, int iscw)
{
	float t, d;
	fz_point p;

	while (th1 < th0)
		th1 += (float)M_PI * 2;

	d = (float)M_PI / 180; /* 1-degree precision */

	if (iscw)
	{
		for (t = th0 + d; t < th1 - d/2; t += d)
		{
			p.x = cosf(t);
			p.y = sinf(t);
			fz_transform_point(&p, mtx);
			fz_lineto(doc, path, p.x, p.y);
		}
	}
	else
	{
		th0 += (float)M_PI * 2;
		for (t = th0 - d; t > th1 + d/2; t -= d)
		{
			p.x = cosf(t);
			p.y = sinf(t);
			fz_transform_point(&p, mtx);
			fz_lineto(doc, path, p.x, p.y);
		}
	}
}