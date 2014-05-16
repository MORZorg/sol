func prog( a, b: int; c: string; ): int
	type	T1: int;
			T2: string;
			T3, T4: T2;
			T5: vector [ 10 ] of T3;
			
	var		i, j: int;
			z: T1;
			s, t: T5;
            q: struct( a: int; b: T4; );
			a: vector [ 100 ] of int;
			
	const 	MAX: int = 100;
			NAME: T2 = "alpha";
			VECT: vector [ 5 ] of real = vector( 2.0, 3.12, 4.67, 1.1, 23.0 );
			MAT: vector [ 2 ] of vector [ 5 ] of real = vector( VECT, vector( x, y, z, 10.0, x + y * -z ) );

begin prog
    a = 1;
    s[ 5 ][ 3 ] = 5.6;
    q.b = "Carl";
    if q.b == "ciao" then
        q.a = 4;
    elsif not false != false then
        q.a = 3;
    else
        q.a = b;
        return prog(q.a, a, q.b);
    endif;

    while q.a >= 0 do
        q.a = q.a + -1;
    endwhile;

    for j = 4 / 2 to toint( 3 / 1 * 1 ) do
        a[ j ] = j;
    endfor;

    foreach j in a do
        a[ j ] = a[ j ];
    endforeach;

    read [ "asd" ] z;
    write [ "asd" + "pino" ] t;
	write if a > 3 then toint( a ) else toreal( a ) endif;

    return i;
end prog
