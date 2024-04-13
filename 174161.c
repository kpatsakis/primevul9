		std::codecvt_base::result do_in( std::mbstate_t& state,
			const char* from,
			const char* from_end,
			const char*& from_next,
			wchar_t* to,
			wchar_t* to_end,
			wchar_t*& to_next ) const
		{
			try
			{
				customcodecvt_do_conversion<char, wchar_t>(state, from, from_end, from_next, to, to_end, to_next);
			}
			catch(...)
			{
				ERR_FS << "Invalid UTF-8 string'" << std::string(from, from_end) << "' " << std::endl;
				return std::codecvt_base::error;
			}
			return std::codecvt_base::ok;
		}