e_weak_ref_new (gpointer object)
{
	GWeakRef *weak_ref;

	weak_ref = g_slice_new0 (GWeakRef);
	g_weak_ref_init (weak_ref, object);

	return weak_ref;
}