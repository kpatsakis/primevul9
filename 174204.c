		int do_length( std::mbstate_t& /*state*/,
			const char* /*from*/,
			const char* /*from_end*/,
			std::size_t /*max*/ ) const
		{
			//Not used by boost filesystem
			throw "Not supported";
		}