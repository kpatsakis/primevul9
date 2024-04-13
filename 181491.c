	LowFirstBitReader(BufferedTransformation &store)
		: m_store(store), m_buffer(0), m_bitsBuffered(0) {}