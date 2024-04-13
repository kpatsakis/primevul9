convert_cmyk_to_rgb (struct jpeg_decompress_struct *cinfo,
		     guchar **lines) 
{
	gint i, j;

	g_return_if_fail (cinfo != NULL);
	g_return_if_fail (cinfo->output_components == 4);
	g_return_if_fail (cinfo->out_color_space == JCS_CMYK);

	for (i = cinfo->rec_outbuf_height - 1; i >= 0; i--) {
		guchar *p;
		
		p = lines[i];
		for (j = 0; j < cinfo->output_width; j++) {
			int c, m, y, k;
			c = p[0];
			m = p[1];
			y = p[2];
			k = p[3];

			/* We now assume that all CMYK JPEG files
			 * use inverted CMYK, as Photoshop does
			 * See https://bugzilla.gnome.org/show_bug.cgi?id=618096 */
			p[0] = k*c / 255;
			p[1] = k*m / 255;
			p[2] = k*y / 255;
			p[3] = 255;
			p += 4;
		}
	}
}