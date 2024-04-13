void ContentLine_Analyzer::InitState()
	{
	flag_NULs = 0;
	CR_LF_as_EOL = (CR_as_EOL | LF_as_EOL);
	skip_deliveries = 0;
	skip_partial = 0;
	buf = 0;
	seq_delivered_in_lines = 0;
	skip_pending = 0;
	seq = 0;
	seq_to_skip = 0;
	plain_delivery_length = 0;
	is_plain = 0;
	suppress_weirds = false;

	InitBuffer(0);
	}