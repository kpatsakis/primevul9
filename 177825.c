static bool is_mounted_with_shared_option(const char *dir)
{
	struct sc_mountinfo *sm SC_CLEANUP(sc_cleanup_mountinfo) = NULL;
	sm = sc_parse_mountinfo(NULL);
	if (sm == NULL) {
		die("cannot parse /proc/self/mountinfo");
	}
	struct sc_mountinfo_entry *entry = sc_first_mountinfo_entry(sm);
	while (entry != NULL) {
		const char *mount_dir = entry->mount_dir;
		if (sc_streq(mount_dir, dir)) {
			const char *optional_fields = entry->optional_fields;
			if (strstr(optional_fields, "shared:") != NULL) {
				return true;
			}
		}
		entry = sc_next_mountinfo_entry(entry);
	}
	return false;
}