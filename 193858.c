void ContentLine_Analyzer::SkipBytes(int64_t length)
	{
	skip_pending = 0;
	seq_to_skip = SeqDelivered() + length;
	}