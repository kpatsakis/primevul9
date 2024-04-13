bool ValidateSHACAL2()
{
	std::cout << "\nSHACAL-2 validation suite running...\n\n";
	bool pass1 = true, pass2 = true, pass3 = true;

	SHACAL2Encryption enc;  // 128 to 512-bits (16 to 64-bytes)
	pass1 = enc.StaticGetValidKeyLength(8) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(15) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(64) == 64 && pass1;
	pass1 = enc.StaticGetValidKeyLength(65) == 64 && pass1;
	pass1 = enc.StaticGetValidKeyLength(128) == 64 && pass1;
	pass1 = enc.StaticGetValidKeyLength(0) == enc.MinKeyLength() && pass1;
	pass1 = enc.StaticGetValidKeyLength(SIZE_MAX) == enc.MaxKeyLength() && pass1;

	SHACAL2Decryption dec;  // 128 to 512-bits (16 to 64-bytes)
	pass2 = dec.StaticGetValidKeyLength(8) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(15) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(64) == 64 && pass2;
	pass2 = dec.StaticGetValidKeyLength(65) == 64 && pass2;
	pass2 = dec.StaticGetValidKeyLength(128) == 64 && pass2;
	pass2 = dec.StaticGetValidKeyLength(0) == dec.MinKeyLength() && pass2;
	pass2 = dec.StaticGetValidKeyLength(SIZE_MAX) == dec.MaxKeyLength() && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/shacal2v.dat", true, new HexDecoder);
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<SHACAL2Encryption, SHACAL2Decryption>(16), valdata, 4) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<SHACAL2Encryption, SHACAL2Decryption>(64), valdata, 10) && pass3;
	return pass1 && pass2 && pass3;
}