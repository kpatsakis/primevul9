imapx_weak_ref_new (gpointer object)
{
	GWeakRef *weak_ref;

	/* XXX Might want to expose this in Camel's public API if it
	 *     proves useful elsewhere.  Based on e_weak_ref_new(). */

	weak_ref = g_slice_new0 (GWeakRef);
	g_weak_ref_init (weak_ref, object);

	return weak_ref;
}