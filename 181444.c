	size_t Put2(const byte *inString, size_t length, int messageEnd, bool blocking)
	{
		CRYPTOPP_UNUSED(messageEnd), CRYPTOPP_UNUSED(blocking);

		while (length--)
			FilterTester::PutByte(*inString++);

		if (messageEnd)
			if (counter != outputLen)
			{
				fail = true;
				CRYPTOPP_ASSERT(false);
			}

		return 0;
	}