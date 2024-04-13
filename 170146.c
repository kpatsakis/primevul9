	TEST_METHOD(2) {
		// The (string) constructor creates a ServerInstanceDir object that's
		// associated with the given directory, and creates the directory
		// if it doesn't exist.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir dir2(dir.getPath());
		ServerInstanceDir dir3(parentDir + "/foo");
		ensure_equals(dir2.getPath(), dir.getPath());
		ensure_equals(dir3.getPath(), parentDir + "/foo");
		ensure_equals(getFileType(dir3.getPath()), FT_DIRECTORY);
	}