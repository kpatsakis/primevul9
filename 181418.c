	BlockTransformation* NewDecryption(const byte *keyStr) const
		{return new D(keyStr, m_keylen, m_rounds);}