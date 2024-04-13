bool ValidateSquare()
{
	std::cout << "\nSquare validation suite running...\n\n";
	bool pass1 = true, pass2 = true;

	SquareEncryption enc;  // 128-bits only
	pass1 = enc.StaticGetValidKeyLength(8) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(15) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(16) == 16 && pass1;
	pass1 = enc.StaticGetValidKeyLength(17) == 16 && pass1;

	SquareDecryption dec;  // 128-bits only
	pass2 = dec.StaticGetValidKeyLength(8) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(15) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(16) == 16 && pass2;
	pass2 = dec.StaticGetValidKeyLength(17) == 16 && pass2;
	std::cout << (pass1 && pass2 ? "passed:" : "FAILED:") << "  Algorithm key lengths\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/squareva.dat", true, new HexDecoder);
	return BlockTransformationTest(FixedRoundsCipherFactory<SquareEncryption, SquareDecryption>(), valdata) && pass1 && pass2;
}