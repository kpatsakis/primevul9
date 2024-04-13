		static void customcodecvt_do_conversion( std::mbstate_t& /*state*/,
			const char_t_from* from,
			const char_t_from* from_end,
			const char_t_from*& from_next,
			char_t_to* to,
			char_t_to* to_end,
			char_t_to*& to_next )
		{
			typedef typename ucs4_convert_impl::convert_impl<char_t_from>::type impl_type_from;
			typedef typename ucs4_convert_impl::convert_impl<char_t_to>::type impl_type_to;

			from_next = from;
			to_next = to;
			customcodecvt_do_conversion_writer<char_t_to> writer(to_next, to_end);
			while(from_next != from_end)
			{
				impl_type_to::write(writer, impl_type_from::read(from_next, from_end));
			}
		}