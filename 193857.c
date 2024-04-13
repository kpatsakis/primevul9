void ContentLine_Analyzer::Undelivered(uint64 seq, int len, bool orig)
	{
	ForwardUndelivered(seq, len, orig);
	}