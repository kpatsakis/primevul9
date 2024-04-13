		static_runner() {
			// Boost uses the current locale to generate a UTF-8 one
			std::locale utf8_loc = boost::locale::generator().generate("");
			// use a custom locale becasue we want to use out log.hpp functions in case of an invalid string.
			utf8_loc = std::locale(utf8_loc, new customcodecvt());
			boost::filesystem::path::imbue(utf8_loc);
		}