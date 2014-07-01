func dumb(): string

begin dumb
	write struct( "You entered", rd struct( pino:int; beppe:vector[3] of vector[2] of struct( b:string; c:bool; ); acca:char; ) );
	return "lol";
end dumb
