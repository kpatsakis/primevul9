	TEST_METHOD(11) {
		// getGeneration() returns the given generation.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir::GenerationPtr generation0 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation1 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation2 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation3 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		
		ensure_equals(dir.getGeneration(0)->getNumber(), 0u);
		ensure_equals(dir.getGeneration(3)->getNumber(), 3u);
	}