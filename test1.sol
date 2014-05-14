func prog( a, b: int; c: string; ): int
	type	T1: int;
			T2: string;
			T3, T4: T2;
			T5: vector [ 10 ] of T3;
			
	var		i, j: int;
			z: T1;
			s, t: T5;
			a: vector [ 100 ] of int;
			
	const 	MAX: int = 100;
			NAME: T2 = "alpha";
			VECT: vector [ 5 ] of real = vector( 2.0, 3.12, 4.67, 1.1, 23.0 );
			MAT: vector [ 2 ] of vector [ 5 ] of real = vector( VECT, vector( x, y, z, 10,0, x + y * z ) );

begin prog
	write MAX;
end prog
