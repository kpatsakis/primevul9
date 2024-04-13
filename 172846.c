static inline int get_op(char **pos){
	while (**pos && !(is_op (**pos) && !is_var (*pos))) {
		(*pos)++;
	}
	return (**pos)? ((is_op (**pos)) + 1): 0;
}