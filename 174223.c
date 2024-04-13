std::string get_program_invocation(const std::string &program_name)
{
	const std::string real_program_name(program_name
#ifdef DEBUG
		+ "-debug"
#endif
#ifdef _WIN32
		+ ".exe"
#endif
	);
	return (path(game_config::wesnoth_program_dir) / real_program_name).string();
}