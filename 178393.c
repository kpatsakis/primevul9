hsb2rgb (float h, float s, float v, guchar *red, guchar *green, guchar *blue)
{
        float f, p, q, t, r, g, b;
        int i;

        s /= 100;
        v /= 100;
        h /= 60;
        i = floor (h);
        if (i == 6)
                i = 0;
        else if ((i > 6) || (i < 0))
                return FALSE;
        f = h - i;
        p = v * (1 - s);
        q = v * (1 - (s * f));
        t = v * (1 - (s * (1 - f)));
        r = g = b = 0;

	if (i == 0) {
		r = v;
		g = t;
		b = p;
	} else if (i == 1) {
		r = q;
		g = v;
		b = p;
	} else if (i == 2) {
		r = p;
		g = v;
		b = t;
	} else if (i == 3) {
		r = p;
		g = q;
		b = v;
	} else if (i == 4) {
		r = t;
		g = p;
		b = v;
	} else if (i == 5) {
		r = v;
		g = p;
		b = q;
	}

        *red   = (guchar)floor(r * 255.0);
        *green = (guchar)floor(g * 255.0);
        *blue  = (guchar)floor(b * 255.0);
	
        return TRUE;
}