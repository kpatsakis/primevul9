alloc_path_with_tree_prefix(__le16 **out_path, int *out_size, int *out_len,
			    const char *treename, const __le16 *path)
{
	int treename_len, path_len;
	struct nls_table *cp;
	const __le16 sep[] = {cpu_to_le16('\\'), cpu_to_le16(0x0000)};

	/*
	 * skip leading "\\"
	 */
	treename_len = strlen(treename);
	if (treename_len < 2 || !(treename[0] == '\\' && treename[1] == '\\'))
		return -EINVAL;

	treename += 2;
	treename_len -= 2;

	path_len = UniStrnlen((wchar_t *)path, PATH_MAX);

	/*
	 * make room for one path separator between the treename and
	 * path
	 */
	*out_len = treename_len + 1 + path_len;

	/*
	 * final path needs to be null-terminated UTF16 with a
	 * size aligned to 8
	 */

	*out_size = roundup((*out_len+1)*2, 8);
	*out_path = kzalloc(*out_size, GFP_KERNEL);
	if (!*out_path)
		return -ENOMEM;

	cp = load_nls_default();
	cifs_strtoUTF16(*out_path, treename, treename_len, cp);
	UniStrcat(*out_path, sep);
	UniStrcat(*out_path, path);
	unload_nls(cp);

	return 0;
}