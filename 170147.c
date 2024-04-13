	TEST_METHOD(4) {
		// The destructor does not throw any exceptions if the server instance
		// directory doesn't exist anymore.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		removeDirTree(dir.getPath());
	}