void ContentLine_Analyzer::CheckNUL()
	{
	// If this is the first byte seen on this connection,
	// and if the connection's state is PARTIAL, then we've
	// intercepted a keep-alive, and shouldn't complain
	// about it.  Note that for PARTIAL connections, the
	// starting sequence number is adjusted as though there
	// had been an initial SYN, so we check for whether
	// the connection has at most two bytes so far.

	TCP_Analyzer* tcp =
		static_cast<TCP_ApplicationAnalyzer*>(Parent())->TCP();

	if ( tcp )
		{
		TCP_Endpoint* endp = IsOrig() ? tcp->Orig() : tcp->Resp();
		if ( endp->state == TCP_ENDPOINT_PARTIAL &&
		     endp->LastSeq() - endp->StartSeq() <= 2 )
			; // Ignore it.
		else
			{
			if ( ! suppress_weirds && Conn()->FlagEvent(NUL_IN_LINE) )
				Conn()->Weird("NUL_in_line");
			flag_NULs = 0;
			}
		}
	}