void ContentLine_Analyzer::SetPlainDelivery(int64_t length)
	{
	if ( length < 0 )
		{
		reporter->AnalyzerError(this,
		                                "negative length for plain delivery");
		return;
		}

	plain_delivery_length = length;
	}