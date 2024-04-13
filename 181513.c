bool ValidateMARS()
{
	std::cout << "\nMARS validation suite running...\n\n";
	bool pass1 = true, pass2 = true, pass3 = true;

	MARSEncryption enc;
	pass1 = enc.StaticGetValidKeyLength(8) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(24) == 24 && pass1;
	pass1 = enc.StaticGetValidKeyLength(32) == 32 && pass1;
	pass1 = enc.StaticGetValidKeyLength(64) == 56 && pass1;
	pass1 = enc.StaticGetValidKeyLength(128) == 56 && pass1;
	pass1 = enc.StaticGetValidKeyLength(0) == enc.MinKeyLength() && pass1;
	pass1 = enc.StaticGetValidKeyLength(SIZE_MAX) == enc.MaxKeyLength() && pass1;

	MARSDecryption dec;
	pass2 = dec.StaticGetValidKeyLength(8) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(24) == 24 && pass2;
	pass2 = dec.StaticGetValidKeyLength(32) == 32 && pass2;
	pass2 = dec.StaticGetValidKeyLength(64) == 56 && pass2;
	pass2 = dec.StaticGetValidKeyLength(128) == 56 && pass2;
	pass2 = dec.StaticGetValidKeyLength(0) == dec.MinKeyLength() && pass2;
	pass2 = dec.StaticGetValidKeyLength(SIZE_MAX) == dec.MaxKeyLength() && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/marsval.dat", true, new HexDecoder);
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<MARSEncryption, MARSDecryption>(16), valdata, 4) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<MARSEncryption, MARSDecryption>(24), valdata, 3) && pass3;
	pass3 = BlockTransformationTest(FixedRoundsCipherFactory<MARSEncryption, MARSDecryption>(32), valdata, 2) && pass3;
	return pass1 && pass2 && pass3;
}