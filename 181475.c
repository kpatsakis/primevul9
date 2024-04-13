bool ValidateAll(bool thorough)
{
	bool pass=TestSettings();
	pass=TestOS_RNG() && pass;
	pass=TestRandomPool() && pass;
#if !defined(NO_OS_DEPENDENCE)
	pass=TestAutoSeededX917() && pass;
#endif
	// pass=TestSecRandom() && pass;
#if defined(CRYPTOPP_EXTENDED_VALIDATION)
	pass=TestMersenne() && pass;
#endif
#if (CRYPTOPP_BOOL_X86 || CRYPTOPP_BOOL_X32 || CRYPTOPP_BOOL_X64)
	pass=TestRDRAND() && pass;
	pass=TestRDSEED() && pass;
#endif

#if defined(CRYPTOPP_EXTENDED_VALIDATION)
	// http://github.com/weidai11/cryptopp/issues/92
	pass=TestSecBlock() && pass;
	// http://github.com/weidai11/cryptopp/issues/336
	pass=TestIntegerBitops() && pass;
	// http://github.com/weidai11/cryptopp/issues/64
	pass=TestPolynomialMod2() && pass;
	// http://github.com/weidai11/cryptopp/issues/360
	pass=TestRounding() && pass;
	// http://github.com/weidai11/cryptopp/issues/242
	pass=TestHuffmanCodes() && pass;
	// http://github.com/weidai11/cryptopp/issues/346
	pass=TestASN1Parse() && pass;
	// Additional tests due to no coverage
	pass=ValidateBaseCode() && pass;
	pass=TestCompressors() && pass;
	pass=TestSharing() && pass;
	pass=TestEncryptors() && pass;
#endif

	pass=ValidateCRC32() && pass;
	pass=ValidateCRC32C() && pass;
	pass=ValidateAdler32() && pass;
	pass=ValidateMD2() && pass;
#if defined(CRYPTOPP_EXTENDED_VALIDATION)
	pass=ValidateMD4() && pass;
#endif
	pass=ValidateMD5() && pass;
	pass=ValidateSHA() && pass;

	pass=RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/keccak.txt") && pass;
	pass=RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/sha3.txt") && pass;

	pass=ValidateHashDRBG() && pass;
	pass=ValidateHmacDRBG() && pass;

	pass=ValidateTiger() && pass;
	pass=ValidateRIPEMD() && pass;
	pass=ValidatePanama() && pass;
	pass=ValidateWhirlpool() && pass;

	pass=ValidateBLAKE2s() && pass;
	pass=ValidateBLAKE2b() && pass;
	pass=ValidatePoly1305() && pass;
	pass=ValidateSipHash() && pass;

	pass=ValidateHMAC() && pass;
	pass=ValidateTTMAC() && pass;

	pass=ValidatePBKDF() && pass;
	pass=ValidateHKDF() && pass;

	pass=ValidateDES() && pass;
	pass=ValidateCipherModes() && pass;
	pass=ValidateIDEA() && pass;
	pass=ValidateSAFER() && pass;
	pass=ValidateRC2() && pass;
	pass=ValidateARC4() && pass;
	pass=ValidateRC5() && pass;
	pass=ValidateBlowfish() && pass;
	pass=ValidateThreeWay() && pass;
	pass=ValidateGOST() && pass;
	pass=ValidateSHARK() && pass;
	pass=ValidateCAST() && pass;
	pass=ValidateSquare() && pass;
	pass=ValidateSKIPJACK() && pass;
	pass=ValidateSEAL() && pass;
	pass=ValidateRC6() && pass;
	pass=ValidateMARS() && pass;
	pass=ValidateRijndael() && pass;
	pass=ValidateTwofish() && pass;
	pass=ValidateSerpent() && pass;
	pass=ValidateSHACAL2() && pass;
	pass=ValidateARIA() && pass;
	pass=ValidateCamellia() && pass;
	pass=ValidateSalsa() && pass;
	pass=ValidateSosemanuk() && pass;
	pass=RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/seed.txt") && pass;
	pass=RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/kalyna.txt") && pass;
	pass=ValidateVMAC() && pass;
	pass=ValidateCCM() && pass;
	pass=ValidateGCM() && pass;
	pass=ValidateCMAC() && pass;
	pass=RunTestDataFile(CRYPTOPP_DATA_DIR "TestVectors/eax.txt") && pass;

	pass=ValidateBBS() && pass;
	pass=ValidateDH() && pass;
	pass=ValidateMQV() && pass;
	pass=ValidateHMQV() && pass;
	pass=ValidateFHMQV() && pass;
	pass=ValidateRSA() && pass;
	pass=ValidateElGamal() && pass;
	pass=ValidateDLIES() && pass;
	pass=ValidateNR() && pass;
	pass=ValidateDSA(thorough) && pass;
	pass=ValidateLUC() && pass;
	pass=ValidateLUC_DH() && pass;
	pass=ValidateLUC_DL() && pass;
	pass=ValidateXTR_DH() && pass;
	pass=ValidateRabin() && pass;
	pass=ValidateRW() && pass;
//	pass=ValidateBlumGoldwasser() && pass;
	pass=ValidateECP() && pass;
	pass=ValidateEC2N() && pass;
	pass=ValidateECDSA() && pass;
	pass=ValidateECGDSA() && pass;
	pass=ValidateESIGN() && pass;

	if (pass)
		std::cout << "\nAll tests passed!\n";
	else
		std::cout << "\nOops!  Not all tests passed.\n";

	return pass;
}