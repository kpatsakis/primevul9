mono_class_interface_implements_interface (MonoClass *candidate, MonoClass *iface)
{
	MonoError error;
	int i;
	do {
		if (candidate == iface)
			return TRUE;
		mono_class_setup_interfaces (candidate, &error);
		if (!mono_error_ok (&error)) {
			mono_error_cleanup (&error);
			return FALSE;
		}

		for (i = 0; i < candidate->interface_count; ++i) {
			if (candidate->interfaces [i] == iface || mono_class_interface_implements_interface (candidate->interfaces [i], iface))
				return TRUE;
		}
		candidate = candidate->parent;
	} while (candidate);
	return FALSE;
}