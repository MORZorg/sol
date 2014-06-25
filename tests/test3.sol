func counter(): int

	var i, j: int;

begin counter
	for i = 0 to 10 do
		for j = 0 to 10 do
			write i + j;
		endfor;
	endfor;
	return 0;
end counter
