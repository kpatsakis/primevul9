		void createGenerationDir(const string &instanceDir, unsigned int number) {
			string command = "mkdir " + instanceDir + "/generation-" + toString(number);
			runShellCommand(command.c_str());
		}