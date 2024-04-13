bool ValidateSKIPJACK()
{
	std::cout << "\nSKIPJACK validation suite running...\n\n";
	bool pass1 = true, pass2 = true;

	SKIPJACKEncryption enc;  // 80-bits only
	pass1 = enc.StaticGetValidKeyLength(8) == 10 && pass1;
	pass1 = enc.StaticGetValidKeyLength(9) == 10 && pass1;
	pass1 = enc.StaticGetValidKeyLength(10) == 10 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 10 && pass1;

	SKIPJACKDecryption dec;  // 80-bits only
	pass2 = dec.StaticGetValidKeyLength(8) == 10 && pass2;
	pass2 = dec.StaticGetValidKeyLength(9) == 10 && pass2;
	pass2 = dec.StaticGetValidKeyLength(10) == 10 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 10 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/skipjack.dat", true, new HexDecoder);
	return BlockTransformationTest(FixedRoundsCipherFactory<SKIPJACKEncryption, SKIPJACKDecryption>(), valdata) && pass1 && pass2;
}