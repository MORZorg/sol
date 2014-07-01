func main(): int
	type  str: vector[ 15 ] of char;
		  permutation: struct( str: str; perm: int; );
	var	  i: int;
		  p: permutation;
	const MAX_LEN: int = 15;

	func strlen( s: str; ): int
		var i: int;
	begin strlen
		for i = 0 to MAX_LEN do
			if s[ i ] == '\0' then
				break;
			endif;
		endfor;

		return i;
	end strlen

	func strcat( s1, s2: str; ): str
		var result, s: str;
			c: char;
	begin strcat
		if strlen( s1 ) + strlen( s2 ) <= MAX_LEN then
			foreach s in vector( s1, s2 ) do
				foreach c in s do
					result[ strlen( result ) ] = c;
				endforeach;
			endforeach;
		endif;

		return result;
	end strcat

	func factorial( i: int; ): int
		var result: int;
	begin factorial
		result = 1;
		for i = 2 to i+1 do
			result = result * i;
		endfor;

		return result;
	end factorial

	func ceil( r: real; ): int
		var result: int;
	begin ceil
		result = toint( r );
		if toreal( result ) > r then
			result = result - 1;
		endif;

		return result;
	end ceil

	func mod( a, b: int; ): int
	begin mod
		return a - ceil( toreal( a ) / toreal( b ) ) * b;
	end mod

	func next_permutation( p: permutation; ): permutation
		var result: permutation;
			temp: char;
			len, i: int;
	begin next_permutation
		len = strlen( p.str );
		if p.perm == factorial( len ) then
			p.perm = 0;
		endif;

		p.perm = p.perm + 1;

		for i = 2 to len+1 do
			if mod( p.perm, factorial( i ) ) != 0 then
				temp = p.str[ len - 1 ];
				p.str[ len - 1 ] = p.str[ len - i ];
				p.str[ len - i ] = temp;
				break;
			endif;
		endfor;

		return p;
	end next_permutation

begin main
	p.str = rd str;
	for i = 0 to factorial( strlen( str ) ) do
		p = wr next_permutation( p );
	endfor;
	return 0;
end main

