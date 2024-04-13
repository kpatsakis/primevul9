		ServerInstanceDirTest(): tmpDir("server_instance_dir_test.tmp") {
			parentDir = "server_instance_dir_test.tmp";
			nobodyGroup = getPrimaryGroupName("nobody");
		}