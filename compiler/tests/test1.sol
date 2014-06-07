func prog( a, b: int; c: string; ): int
	type	T1: int;
			BLANDO: real;
			T2: string;
			T3, T4: T2;
			T5: vector [ 10 ] of T3;

	var		i, j: int;
			x, y: BLANDO;
			z: T1;
			s, t: T5;
            useful: struct( a: struct( a: int; ); b: struct( b: int; ); );
			q: struct( a: int; b: T4; );
			d: vector [ 100 ] of int;
			carl: struct( lenny: int; );

	const 	MAX: int = 100;
			PINO: struct( a, b: int; ) = struct( 3, 5 );
			NAME: T2 = "alpha";
			VECT: vector [ 5 ] of real = vector( 2.0, 3.12, 4.67, 1.1, 23.0 );
			MAT: vector [ 2 ] of vector [ 5 ] of real = vector( VECT, vector( x, y, toreal( toint( toreal( z ) ) ), 10.0, x + y * toreal( -z ) ) );

	func asd(): int
	begin asd
		return 1;
	end asd

begin prog
	a = 1;
	-- s[ 5 ][ 3 ] = 5.6;
	s[ 5 ] = "Carlito";
	q.b = "Carl";
	q.b = "Ca\trl";
	carl.lenny = 1;
	if q.b == "ciao" then
		q.a = 4;
		d[0] = 1;
	elsif not false != false then
		q.a = 3;
	else
		q.a = b;
		return prog(q.a, q.a, q.b);
	endif;

	while q.a >= 0 do
		q.a = q.a + -1;
	endwhile;

	for j = 4 / 2 to 3 / 1 * toint( 1.1 ) do
		d[ j ] = j;
	endfor;

	foreach j in d do
		d[ j ] = -d[ j ];
		j = -j;
	endforeach;

	read [ "asd" ] z;
	write [ "asdpino" ] t;
	write if a > 3 then toreal( a ) elsif a > 2 then toreal( a + 0 ) elsif a > 1 then toreal( a + 1 - 1 ) else toreal( a * 1 ) endif;

	return i;
end prog
