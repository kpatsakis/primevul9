bool TestRandomPool()
{
	std::cout << "\nTesting RandomPool generator...\n\n";
	bool pass=true, fail;
	{
		RandomPool prng;
		static const unsigned int ENTROPY_SIZE = 32;

		MeterFilter meter(new Redirector(TheBitBucket()));
		RandomNumberSource test(prng, 100000, true, new Deflator(new Redirector(meter)));

		fail = false;
		if (meter.GetTotalBytes() < 100000)
			fail = true;

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  100000 generated bytes compressed to " << meter.GetTotalBytes() << " bytes by DEFLATE\n";

		try
		{
			fail = false;
			prng.DiscardBytes(100000);
		}
		catch (const Exception&)
		{
			fail = true;
		}

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  discarded 10000 bytes" << std::endl;

		try
		{
			fail = false;
			if(prng.CanIncorporateEntropy())
			{
				SecByteBlock entropy(ENTROPY_SIZE);
				GlobalRNG().GenerateBlock(entropy, entropy.SizeInBytes());

				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
			}
		}
		catch (const Exception& /*ex*/)
		{
			fail = true;
		}

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  IncorporateEntropy with " << 4*ENTROPY_SIZE << " bytes\n";

		try
		{
			// Miscellaneous for code coverage
			(void)prng.AlgorithmName();  // "unknown"
			word32 result = prng.GenerateWord32();
			result = prng.GenerateWord32((result & 0xff), 0xffffffff - (result & 0xff));
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 4);
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 3);
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 2);
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 1);
		}
		catch (const Exception&)
		{
			fail = true;
		}

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  GenerateWord32 and Crop\n";
	}

#if !defined(NO_OS_DEPENDENCE)
	std::cout << "\nTesting AutoSeeded RandomPool generator...\n\n";
	{
		AutoSeededRandomPool prng;
		static const unsigned int ENTROPY_SIZE = 32;

		MeterFilter meter(new Redirector(TheBitBucket()));
		RandomNumberSource test(prng, 100000, true, new Deflator(new Redirector(meter)));

		fail = false;
		if (meter.GetTotalBytes() < 100000)
			fail = true;

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  100000 generated bytes compressed to " << meter.GetTotalBytes() << " bytes by DEFLATE\n";

		try
		{
			fail = false;
			prng.DiscardBytes(100000);
		}
		catch (const Exception&)
		{
			fail = true;
		}

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  discarded 10000 bytes" << std::endl;

		try
		{
			fail = false;
			if(prng.CanIncorporateEntropy())
			{
				SecByteBlock entropy(ENTROPY_SIZE);
				GlobalRNG().GenerateBlock(entropy, entropy.SizeInBytes());

				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
				prng.IncorporateEntropy(entropy, entropy.SizeInBytes());
			}
		}
		catch (const Exception& /*ex*/)
		{
			fail = true;
		}

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  IncorporateEntropy with " << 4*ENTROPY_SIZE << " bytes\n";

		try
		{
			// Miscellaneous for code coverage
			fail = false;
			(void)prng.AlgorithmName();  // "unknown"
			word32 result = prng.GenerateWord32();
			result = prng.GenerateWord32((result & 0xff), 0xffffffff - (result & 0xff));
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 4);
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 3);
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 2);
			prng.GenerateBlock(reinterpret_cast<byte*>(&result), 1);
		}
		catch (const Exception&)
		{
			fail = true;
		}

		pass &= !fail;
		if (fail)
			std::cout << "FAILED:";
		else
			std::cout << "passed:";
		std::cout << "  GenerateWord32 and Crop\n";
	}
#endif

	std::cout.flush();
	return pass;
}