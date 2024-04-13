	BlockTransformation* NewEncryption(const byte *keyStr) const
		{return new E(keyStr, m_keylen, m_rounds);}