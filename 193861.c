int ContentLine_Analyzer::HasPartialLine() const
	{
	return buf && offset > 0;
	}