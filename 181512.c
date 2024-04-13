	HuffmanDecoder * operator()() const
	{
		unsigned int codeLengths[32];
		std::fill(codeLengths + 0, codeLengths + 32, 5);
		member_ptr<HuffmanDecoder> pDecoder(new HuffmanDecoder);
		pDecoder->Initialize(codeLengths, 32);
		return pDecoder.release();
	}