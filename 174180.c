		std::codecvt_base::result do_out( std::mbstate_t& state,
			const wchar_t* from,
			const wchar_t* from_end,
			const wchar_t*& from_next,
			char* to,
			char* to_end,
			char*& to_next ) const
		{
			try
			{
				customcodecvt_do_conversion<wchar_t, char>(state, from, from_end, from_next, to, to_end, to_next);
			}
			catch(...)
			{
				ERR_FS << "Invalid UTF-16 string" << std::endl;
				return std::codecvt_base::error;	
			}
			return std::codecvt_base::ok;
		}