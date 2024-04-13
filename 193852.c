void ContentLine_Analyzer::DeliverStream(int len, const u_char* data,
						bool is_orig)
	{
	TCP_SupportAnalyzer::DeliverStream(len, data, is_orig);

	if ( len <= 0 || SkipDeliveries() )
		return;

	if ( skip_partial )
		{
		TCP_Analyzer* tcp =
			static_cast<TCP_ApplicationAnalyzer*>(Parent())->TCP();

		if ( tcp && tcp->IsPartial() )
			return;
		}

	if ( buf && len + offset >= buf_len )
		{ // Make sure we have enough room to accommodate the new stuff.
		int old_buf_len = buf_len;
		buf_len = ((offset + len) * 3) / 2 + 1;

		u_char* tmp = new u_char[buf_len];
		for ( int i = 0; i < old_buf_len; ++i )
			tmp[i] = buf[i];

		delete [] buf;
		buf = tmp;
		}

	DoDeliver(len, data);

	seq += len;
	}