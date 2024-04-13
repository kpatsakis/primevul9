static void set_seg_override(struct decode_cache *c, int seg)
{
	c->has_seg_override = true;
	c->seg_override = seg;
}