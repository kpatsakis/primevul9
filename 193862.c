ContentLine_Analyzer::ContentLine_Analyzer(Connection* conn, bool orig)
: TCP_SupportAnalyzer("CONTENTLINE", conn, orig)
	{
	InitState();
	}