	void PutByte(byte inByte)
	{
		if (counter >= outputLen || validOutput[counter] != inByte)
		{
			std::cerr << "incorrect output " << counter << ", " << (word16)validOutput[counter] << ", " << (word16)inByte << "\n";
			fail = true;
			CRYPTOPP_ASSERT(false);
		}
		counter++;
	}