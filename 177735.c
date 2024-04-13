LoadPage::LoadErrorHandling strToLoadErrorHandling(const char * s, bool * ok) {
	if (ok) *ok = true;
	if (!strcasecmp(s, "abort")) return LoadPage::abort;
	if (!strcasecmp(s, "skip")) return LoadPage::skip;
	if (!strcasecmp(s, "ignore")) return LoadPage::ignore;
	*ok = false;
	return LoadPage::abort;
}