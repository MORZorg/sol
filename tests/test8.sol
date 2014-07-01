func main() : int 
	type pseudo_string: vector [ 15 ] of char;

	var word, empty: pseudo_string;
		i: int;

	const	MAX_LEN: int = 15;
			INTRO: string = "Insert a word:";

	func strlen( str: pseudo_string; ) : int
		var i: int;
	begin strlen
		for i = 0 to MAX_LEN do
			if( str[ i ] == '\0' ) then
				break;
			endif;
		endfor;
		return i;
	end strlen

	func strcat( str1, str2: pseudo_string; ) : pseudo_string
		var i, j: int;
	begin strcat
		if strlen( str1 ) + strlen( str2 ) <= MAX_LEN then
			j = 0;
			for i = strlen( str1 ) to strlen( str1 ) + strlen( str2 ) do
				str1[ i ] = str2[ j ];
				j = j + 1;
			endfor;
		endif;

		return str1;
	end strcat

	func permutation( str: pseudo_string; index: int; base: pseudo_string; ) : pseudo_string
		var result, next_step: pseudo_string;
			i, j, next_i: int;
	begin permutation
		-- Returning if i have only one char to process
		if index == 1 then 
			--return str;
			write vector( struct( "Permutation:", strcat( base, str ) ) );
			return strcat( base, str );
		endif;

		j = 0;
		while j < index do
		--	-- Resetting the result
		--	for i = 0 to MAX_LEN do
		--		result[ i ] = '\0';
		--		next_step[ i ] = '\0';
		--	endfor;

			-- Creating the new string to pass as argument
			for i = 0 to index do
				if i != j then
					next_step = strcat( next_step, str[ i ] );
				else
					base = strcat( base, str[ i ] );
				endif;
			endfor;

			-- write vector( struct( "Saving", result ), struct( "Next:", next_step ) );

			-- Decrementing the size of the permutations
			-- result = strcat( base, permutation( next_step, index - 1, base ) );
			write vector( struct( "Out:", permutation( next_step, index - 1, base ) ) );

			j = j + 1;
			--write vector( struct( "Permutation:", strcat( base, result ) ), struct( "Base:", base ), struct( "Result:", result ) );

		endwhile;

		-- Completly wrong
		return result;

	end permutation

begin main
	write INTRO;
	read word;

	write vector( struct( "Word:", word ) );

	for i = 0 to MAX_LEN do
		empty[ i ] = '\0';
	endfor;

	word = permutation( word, strlen( word ), empty );

	return 0;

end main
