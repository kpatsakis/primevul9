get_page_render_details(fz_context *ctx, fz_page *page, render_details *render)
{
	float page_width, page_height;
	int rot;
	float s_x, s_y;

	render->page = page;
	render->list = NULL;
	render->num_workers = num_workers;

	render->bounds = fz_bound_page(ctx, page);
	page_width = (render->bounds.x1 - render->bounds.x0)/72;
	page_height = (render->bounds.y1 - render->bounds.y0)/72;

	s_x = x_resolution / 72;
	s_y = y_resolution / 72;
	if (rotation == -1)
	{
		/* Automatic rotation. If we fit, use 0. If we don't, and 90 would be 'better' use that. */
		if (page_width <= width && page_height <= height)
		{
			/* Page fits, so use no rotation. */
			rot = 0;
		}
		else if (fit)
		{
			/* Use whichever gives the biggest scale */
			float sx_0 = width / page_width;
			float sy_0 = height / page_height;
			float sx_90 = height / page_width;
			float sy_90 = width / page_height;
			float s_0, s_90;
			s_0 = fz_min(sx_0, sy_0);
			s_90 = fz_min(sx_90, sy_90);
			if (s_0 >= s_90)
			{
				rot = 0;
				if (s_0 < 1)
				{
					s_x *= s_0;
					s_y *= s_0;
				}
			}
			else
			{
				rot = 90;
				if (s_90 < 1)
				{
					s_x *= s_90;
					s_y *= s_90;
				}
			}
		}
		else
		{
			/* Use whichever crops the least area */
			float lost0 = 0;
			float lost90 = 0;

			if (page_width > width)
				lost0 += (page_width - width) * (page_height > height ? height : page_height);
			if (page_height > height)
				lost0 += (page_height - height) * page_width;

			if (page_width > height)
				lost90 += (page_width - height) * (page_height > width ? width : page_height);
			if (page_height > width)
				lost90 += (page_height - width) * page_width;

			rot = (lost0 <= lost90 ? 0 : 90);
		}
	}
	else
	{
		rot = rotation;
	}

	render->ctm = fz_pre_scale(fz_rotate(rot), s_x, s_y);
	render->tbounds = fz_transform_rect(render->bounds, render->ctm);;
	render->ibounds = fz_round_rect(render->tbounds);
}