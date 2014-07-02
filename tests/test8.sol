func main() : int 
	type pseudo_string: vector [ 15 ] of char;

	var word, empty: pseudo_string;
		i: int;

	const	MAX_LEN: int = 15;
			INTRO: string = "Insert a word:";

	func new_pseudo_string() : pseudo_string
		var result: pseudo_string;
			i: int;
	begin new_pseudo_string
		for i = 0 to MAX_LEN do
			result[ i ] = '\0';
		endfor;
		return result;
	end new_pseudo_string

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

	func strcpy( str: pseudo_string; ) : pseudo_string
		var result: pseudo_string;
			i: int;
	begin strcpy
		for i = 0 to strlen( str ) do
			result[ i ] = str[ i ];
		endfor;
		return result;
	end strcpy

	func permutation( to_process: pseudo_string; base: pseudo_string; number: int; ) : pseudo_string
		var next_step, current_base, result: pseudo_string;
			i, j: int;
	begin permutation
		-- Returning if i have only one char to process
		if strlen( to_process ) == 1 then 
			write vector( struct( "Permutation:", number + 1, strcat( base, to_process ) ) );
			return strcat( base, to_process );
		endif;

		j = 0;
		while j < strlen( to_process ) do

			-- Resetting base to the argument value and resetting next_step
			current_base = strcpy( base );
			next_step = new_pseudo_string();
			-- Creating the new string to pass as argument
			for i = 0 to strlen( to_process ) do
				if i != j then
					next_step = strcat( next_step, to_process[ i ] );
				else
					current_base = strcat( current_base, to_process[ i ] );
				endif;
			endfor;

		--	write vector(	struct( "Number before", number ),
		--					struct( "Number after", number + strlen( next_step ) * j  ),
		--					struct( "J", j ),
		--					struct( "to_process", strlen( next_step ) ) );
		--	write vector( struct( "Base", base ), struct( "Next step", next_step ) );
			number = number + j * strlen( next_step );
			-- write vector( struct( "Saving", result ), struct( "Next:", next_step ) );

			-- Decrementing the size of the permutations
			-- result = strcat( base, permutation( next_step, index - 1, base ) );
			-- write vector( struct( "Out:", permutation( next_step, base ) ) );
			result = permutation( next_step, current_base, number );

			j = j + 1;
			-- write vector( struct( "Permutation:", strcat( base, result ) ), struct( "Base:", base ), struct( "Result:", result ) );

		endwhile;

		-- Completly useless
		return result;

	end permutation

begin main
	write INTRO;
	read word;

	write vector( struct( "Word:", word ) );

	word = permutation( word, new_pseudo_string(), 0 );

	return 0;

end main
