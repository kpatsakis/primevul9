	TEST_METHOD(14) {
		// It's possible to have two ServerInstanceDir objects constructed
		// with the same (pid_t, string) constructor arguments.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir dir2(parentDir + "/passenger-test.1234");
	}