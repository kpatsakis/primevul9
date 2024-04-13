static void firebird_info_cb(void *arg, char const *s) /* {{{ */
{
	if (arg) {
		if (*(char*)arg) { /* second call */
			strcat(arg, " ");
		}
		strcat(arg, s);
	}
}