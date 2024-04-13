void ContentLine_Analyzer::EndpointEOF(bool is_orig)
	{
	if ( offset > 0 )
		DeliverStream(1, (const u_char*) "\n", is_orig);
	}