static inline void scrub_get_recover(struct scrub_recover *recover)
{
	refcount_inc(&recover->refs);
}