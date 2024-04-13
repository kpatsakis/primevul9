replace_prefix (const gchar *runtime_prefix,
                const gchar *configure_time_path)
{
	return e_util_replace_prefix (
		E_DATA_SERVER_PREFIX, runtime_prefix, configure_time_path);
}