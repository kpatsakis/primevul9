ContentLine_Analyzer::ContentLine_Analyzer(const char* name, Connection* conn, bool orig)
: TCP_SupportAnalyzer(name, conn, orig)
	{
	InitState();
	}