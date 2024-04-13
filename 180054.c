property_equal (MonoProperty *prop1, MonoProperty *prop2)
{
	// Properties are hide-by-name-and-signature
	if (!g_str_equal (prop1->name, prop2->name))
		return FALSE;

	if (prop1->get && prop2->get && !mono_metadata_signature_equal (mono_method_signature (prop1->get), mono_method_signature (prop2->get)))
		return FALSE;
	if (prop1->set && prop2->set && !mono_metadata_signature_equal (mono_method_signature (prop1->set), mono_method_signature (prop2->set)))
		return FALSE;
	return TRUE;
}