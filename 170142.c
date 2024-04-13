	TEST_METHOD(8) {
		// getNewestGeneration() returns the newest generation.
		ServerInstanceDir dir(parentDir + "/passenger-test.1234");
		ServerInstanceDir::GenerationPtr generation0 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation1 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation2 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		ServerInstanceDir::GenerationPtr generation3 = dir.newGeneration(true, "nobody", nobodyGroup, 0, 0);
		
		generation2.reset();
		ensure_equals(dir.getNewestGeneration()->getNumber(), 3u);
		generation3.reset();
		ensure_equals(dir.getNewestGeneration()->getNumber(), 1u);
	}