bool ValidateCMAC()
{
	std::cout << "\nCMAC validation suite running...\n";
	return RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/cmac.txt");
}