	TEST_METHOD(6) {
		// If there are no existing generations, newGeneration() creates a new
		// generation directory with number 0.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		unsigned int ncontents = listDir(dir.getPath()).size();
		ServerInstanceDir::GenerationPtr generation = dir.newGeneration(true,
			"nobody", nobodyGroup, 0, 0);
		
		ensure_equals(generation->getNumber(), 0u);
		ensure_equals(getFileType(generation->getPath()), FT_DIRECTORY);
		ensure_equals(listDir(dir.getPath()).size(), ncontents + 1);
	}