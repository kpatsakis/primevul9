ves_icall_System_Environment_GetLogicalDrives (void)
{
        gunichar2 buf [256], *ptr, *dname;
	gunichar2 *u16;
	guint initial_size = 127, size = 128;
	gint ndrives;
	MonoArray *result;
	MonoString *drivestr;
	MonoDomain *domain = mono_domain_get ();
	gint len;

	MONO_ARCH_SAVE_REGS;

        buf [0] = '\0';
	ptr = buf;

	while (size > initial_size) {
		size = (guint) GetLogicalDriveStrings (initial_size, ptr);
		if (size > initial_size) {
			if (ptr != buf)
				g_free (ptr);
			ptr = g_malloc0 ((size + 1) * sizeof (gunichar2));
			initial_size = size;
			size++;
		}
	}

	/* Count strings */
	dname = ptr;
	ndrives = 0;
	do {
		while (*dname++);
		ndrives++;
	} while (*dname);

	dname = ptr;
	result = mono_array_new (domain, mono_defaults.string_class, ndrives);
	ndrives = 0;
	do {
		len = 0;
		u16 = dname;
		while (*u16) { u16++; len ++; }
		drivestr = mono_string_new_utf16 (domain, dname, len);
		mono_array_setref (result, ndrives++, drivestr);
		while (*dname++);
	} while (*dname);

	if (ptr != buf)
		g_free (ptr);

	return result;
}