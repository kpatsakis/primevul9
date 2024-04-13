		std::codecvt_base::result unshift( std::mbstate_t& /*state*/,
			char* /*to*/,
			char* /*to_end*/,
			char*& /*to_next*/) const
		{
			//Not used by boost filesystem
			throw "Not supported";
		}