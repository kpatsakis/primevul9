	inline bool operator()(const CryptoPP::HuffmanDecoder::CodeInfo &lhs, const CryptoPP::HuffmanDecoder::CodeInfo &rhs)
		{return lhs.code < rhs.code;}