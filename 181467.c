bool ValidateSalsa()
{
	std::cout << "\nSalsa validation suite running...\n";

	return RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/salsa.txt");
}