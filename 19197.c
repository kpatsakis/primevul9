static char *palColorFor(const char *k) {
	if (!r_cons_singleton ()) {
		return NULL;
	}
	RColor rcolor = r_cons_pal_get (k);
	return r_cons_rgb_tostring (rcolor.r, rcolor.g, rcolor.b);
}