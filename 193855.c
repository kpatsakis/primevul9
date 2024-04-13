void ContentLine_Analyzer::SkipBytesAfterThisLine(int64_t length)
	{
	// This is a little complicated because Bro has to handle
	// both CR and CRLF as a line break. When a line is delivered,
	// it's possible that only a CR is seen, and we may not know
	// if an LF is following until we see the next packet.  If an
	// LF follows, we should start skipping bytes *after* the LF.
	// So we keep the skip as 'pending' until we see the next
	// character in DoDeliver().

	if ( last_char == '\r' )
		skip_pending = length;
	else
		SkipBytes(length);
	}