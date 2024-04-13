	TEST_METHOD(9) {
		// getNewestGeneration returns null if there are no generations.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ensure(dir.getNewestGeneration() == NULL);
	}