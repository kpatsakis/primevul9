	inline bool operator()(CryptoPP::HuffmanDecoder::code_t lhs, const CryptoPP::HuffmanDecoder::CodeInfo &rhs)
		{return lhs < rhs.code;}