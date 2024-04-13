bool ValidateIDEA()
{
	std::cout << "\nIDEA validation suite running...\n\n";

	FileSource valdata(CRYPTOPP_DATA_DIR "TestData/ideaval.dat", true, new HexDecoder);
	return BlockTransformationTest(FixedRoundsCipherFactory<IDEAEncryption, IDEADecryption>(), valdata);
}