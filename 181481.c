bool ValidateSosemanuk()
{
	std::cout << "\nSosemanuk validation suite running...\n";
	return RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/sosemanuk.txt");
}