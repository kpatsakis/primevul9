bool TestSettings()
{
	bool pass = true;

	std::cout << "\nTesting Settings...\n\n";

	word32 w;
	const byte s[] = "\x01\x02\x03\x04";

#if (CRYPTOPP_MSC_VERSION >= 1410)
	std::copy(s, s+4,
		stdext::make_checked_array_iterator(reinterpret_cast<byte*>(&w), sizeof(w)));
#else
	std::copy(s, s+4, reinterpret_cast<byte*>(&w));
#endif

	if (w == 0x04030201L)
	{
#ifdef IS_LITTLE_ENDIAN
		std::cout << "passed:  ";
#else
		std::cout << "FAILED:  ";
		pass = false;
#endif
		std::cout << "Your machine is little endian.\n";
	}
	else if (w == 0x01020304L)
	{
#ifndef IS_LITTLE_ENDIAN
		std::cout << "passed:  ";
#else
		std::cout << "FAILED:  ";
		pass = false;
#endif
		std::cout << "Your machine is big endian.\n";
	}
	else
	{
		std::cout << "FAILED:  Your machine is neither big endian nor little endian.\n";
		pass = false;
	}

#if defined(CRYPTOPP_EXTENDED_VALIDATION)
	// App and library versions, http://github.com/weidai11/cryptopp/issues/371
	const int v1 = LibraryVersion();
	const int v2 = HeaderVersion();
	if(v1/10 == v2/10)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "Library version (library): " << v1 << ", header version (app): " << v2 << "\n";
#endif

#ifdef CRYPTOPP_ALLOW_UNALIGNED_DATA_ACCESS
	// Don't assert the alignment of testvals. That's what this test is for.
	byte testvals[10] = {1,2,2,3,3,3,3,2,2,1};
	if (*(word32 *)(void *)(testvals+3) == 0x03030303 && *(word64 *)(void *)(testvals+1) == W64LIT(0x0202030303030202))
		std::cout << "passed:  Your machine allows unaligned data access.\n";
	else
	{
		std::cout << "FAILED:  Unaligned data access gave incorrect results.\n";
		pass = false;
	}
#else
	std::cout << "passed:  CRYPTOPP_ALLOW_UNALIGNED_DATA_ACCESS is not defined. Will restrict to aligned data access.\n";
#endif

	if (sizeof(byte) == 1)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "sizeof(byte) == " << sizeof(byte) << std::endl;

	if (sizeof(word16) == 2)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "sizeof(word16) == " << sizeof(word16) << std::endl;

	if (sizeof(word32) == 4)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "sizeof(word32) == " << sizeof(word32) << std::endl;

	if (sizeof(word64) == 8)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "sizeof(word64) == " << sizeof(word64) << std::endl;

#ifdef CRYPTOPP_WORD128_AVAILABLE
	if (sizeof(word128) == 16)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "sizeof(word128) == " << sizeof(word128) << std::endl;
#endif

	if (sizeof(word) == 2*sizeof(hword)
#ifdef CRYPTOPP_NATIVE_DWORD_AVAILABLE
		&& sizeof(dword) == 2*sizeof(word)
#endif
		)
		std::cout << "passed:  ";
	else
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	std::cout << "sizeof(hword) == " << sizeof(hword) << ", sizeof(word) == " << sizeof(word);
#ifdef CRYPTOPP_NATIVE_DWORD_AVAILABLE
	std::cout << ", sizeof(dword) == " << sizeof(dword);
#endif
	std::cout << std::endl;

#ifdef CRYPTOPP_CPUID_AVAILABLE
	bool hasMMX = HasMMX();
	bool hasISSE = HasISSE();
	bool hasSSE2 = HasSSE2();
	bool hasSSSE3 = HasSSSE3();
	bool hasSSE4 = HasSSE4();
	bool isP4 = IsP4();
	int cacheLineSize = GetCacheLineSize();

	if ((isP4 && (!hasMMX || !hasSSE2)) || (hasSSE2 && !hasMMX) || (cacheLineSize < 16 || cacheLineSize > 256 || !IsPowerOf2(cacheLineSize)))
	{
		std::cout << "FAILED:  ";
		pass = false;
	}
	else
		std::cout << "passed:  ";

	std::cout << "hasMMX == " << hasMMX << ", hasISSE == " << hasISSE << ", hasSSE2 == " << hasSSE2 << ", hasSSSE3 == " << hasSSSE3 << ", hasSSE4 == " << hasSSE4;
	std::cout << ", hasAESNI == " << HasAESNI() << ", hasCLMUL == " << HasCLMUL() << ", hasRDRAND == " << HasRDRAND() << ", hasRDSEED == " << HasRDSEED();
	std::cout << ", hasSHA == " << HasSHA() << ", isP4 == " << isP4 << ", cacheLineSize == " << cacheLineSize << std::endl;

#elif (CRYPTOPP_BOOL_ARM32 || CRYPTOPP_BOOL_ARM64)
	bool hasNEON = HasNEON();
	bool hasPMULL = HasPMULL();
	bool hasCRC32 = HasCRC32();
	bool hasAES = HasAES();
	bool hasSHA1 = HasSHA1();
	bool hasSHA2 = HasSHA2();

	std::cout << "passed:  ";
	std::cout << "hasNEON == " << hasNEON << ", hasPMULL == " << hasPMULL << ", hasCRC32 == " << hasCRC32 << ", hasAES == " << hasAES << ", hasSHA1 == " << hasSHA1 << ", hasSHA2 == " << hasSHA2 << std::endl;
#endif

	if (!pass)
	{
		std::cout << "Some critical setting in config.h is in error.  Please fix it and recompile." << std::endl;
		abort();
	}
	return pass;
}