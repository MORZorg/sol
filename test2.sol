func prog( primo: vector[1] of vector[3] of char; a: struct(a: int;); b: vector[5] of int; ): int

	const PEO: int = 3 + 3 + 4;

    -- I'm a comment, I'm a comment, I'm a comment.
    -- I'm a comment, I'm a comment, I'm a comment.
    func a(): int
    begin a
        return 1;
    end a

begin prog
	write a;
    write 'c';
    write '\t';
    write '\'';
    write prog(primo, a);
    return "a\"\ts\\d";
    while true do
		break;
	endwhile;
end prog

