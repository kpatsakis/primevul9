	HuffmanDecoder * operator()() const
	{
		unsigned int codeLengths[288];
		std::fill(codeLengths + 0, codeLengths + 144, 8);
		std::fill(codeLengths + 144, codeLengths + 256, 9);
		std::fill(codeLengths + 256, codeLengths + 280, 7);
		std::fill(codeLengths + 280, codeLengths + 288, 8);
		member_ptr<HuffmanDecoder> pDecoder(new HuffmanDecoder);
		pDecoder->Initialize(codeLengths, 288);
		return pDecoder.release();
	}