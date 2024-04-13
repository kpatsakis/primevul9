			bool can_push(size_t count)
			{
				return static_cast<size_t>(to_end - to_next) > count;
			}