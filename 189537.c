e_util_generate_uid (void)
{
	static volatile gint counter = 0;
	gchar *uid;
	GChecksum *checksum;

	checksum = g_checksum_new (G_CHECKSUM_SHA1);

	#define add_i64(_x) G_STMT_START { \
		gint64 i64 = (_x); \
		g_checksum_update (checksum, (const guchar *) &i64, sizeof (gint64)); \
	} G_STMT_END

	#define add_str(_x, _def) G_STMT_START { \
		const gchar *str = (_x); \
		if (!str) \
			str = (_def); \
		g_checksum_update (checksum, (const guchar *) str, strlen (str)); \
	} G_STMT_END

	add_i64 (g_get_monotonic_time ());
	add_i64 (g_get_real_time ());
	add_i64 (getpid ());
	add_i64 (getgid ());
	add_i64 (getppid ());
	add_i64 (g_atomic_int_add (&counter, 1));

	add_str (g_get_host_name (), "localhost");
	add_str (g_get_user_name (), "user");
	add_str (g_get_real_name (), "User");

	#undef add_i64
	#undef add_str

	uid = g_strdup (g_checksum_get_string (checksum));

	g_checksum_free (checksum);

	return uid;
}