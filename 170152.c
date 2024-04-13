	TEST_METHOD(3) {
		// A ServerInstanceDir object removes the server instance directory
		// upon destruction, but only if there are no more generations in it.
		{
			ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		}
		ensure_equals(listDir(parentDir).size(), 0u);
		
		{
			ServerInstanceDir dir(parentDir + "/passenger-test.1234");
			createGenerationDir(dir.getPath(), 1);
		}
		ensure_equals(listDir(parentDir).size(), 1u);
	}