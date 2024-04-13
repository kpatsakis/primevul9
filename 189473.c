e_weak_ref_free (GWeakRef *weak_ref)
{
	g_return_if_fail (weak_ref != NULL);

	g_weak_ref_clear (weak_ref);
	g_slice_free (GWeakRef, weak_ref);
}