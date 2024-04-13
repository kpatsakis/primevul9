imapx_weak_ref_free (GWeakRef *weak_ref)
{
	g_return_if_fail (weak_ref != NULL);

	/* XXX Might want to expose this in Camel's public API if it
	 *     proves useful elsewhere.  Based on e_weak_ref_free(). */

	g_weak_ref_clear (weak_ref);
	g_slice_free (GWeakRef, weak_ref);
}