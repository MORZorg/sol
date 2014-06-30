func counter(): int

	var i, j: int;

begin counter
	for i = 0 to 5 do
		if i >= 4 then
			write "I'm done with this shit!";
			break;
		endif;

		for j = 0 to 5 do
			if i < j then 
				break;
			endif;
			write struct( "I", i, "J", j );
		endfor;
		write "Finish innested";
	endfor;
	return i + j;
end counter
