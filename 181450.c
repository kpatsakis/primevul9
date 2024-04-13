	VariableRoundsCipherFactory(unsigned int keylen=0, unsigned int rounds=0)
		: m_keylen(keylen ? keylen : E::DEFAULT_KEYLENGTH), m_rounds(rounds ? rounds : E::DEFAULT_ROUNDS) {}