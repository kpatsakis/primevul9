get_month_str (gint month)
{
	static const gchar tm_months[][4] = {
		"Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
	};

	if (month < 1 || month > 12)
		return NULL;

	return tm_months[month - 1];
}