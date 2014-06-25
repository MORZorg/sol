func game_of_life() : int

	type	lines: vector [ 30 ] of bool;
			grid: vector [ 30 ] of lines;
			-- Just a bad idea.
			-- grid: struct( rows: lines; columns: lines; );

	var		world: grid;


			-- REALLOC ERROR
			const	summary: string = "Welcome to ORZ's Conway's Game of Life";
	--const	summary: string = "Sbra";


			-- Rules:
			--   Any live cell with fewer than two live neighbours dies, as if caused by under-population.
			--   Any live cell with two or three live neighbours lives on to the next generation.
			--   Any live cell with more than three live neighbours dies, as if by overcrowding.
			--   Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
	func next_state() : grid
		var		i, j, k, h: int;
				neighbours: int;
				state: grid;

	begin next_state

		-- FIXME 100 or 99?
		for i = 0 to 100 do
			for j = 0 to 100 do

				neighbours = 0;
				for k = ( if i-1>0 then i-1 else 0 endif ) to ( if i+1<100 then i+1 else 100 endif ) do
					for h = ( if j-1>0 then j-1 else 0 endif ) to ( if j+1<100 then j+1 else 100 endif ) do
						if world[ k ][ h ] then
							neighbours = neighbours + 1;
						else
							neighbours = neighbours;
						endif;
					endfor;
				endfor;

				if world[ i ][ j ] then
					if neighbours < 2  or neighbours > 3 then
						state[ i ][ j ] = false;
					else
						state[ i ][ j ] = true;
					endif;
				else
					if neighbours == 3 then
						state[ i ][ j ] = true;
					else
						state[ i ][ j ] = false;
					endif;
				endif;
			endfor;
		endfor;

		return state;
	end next_state

begin game_of_life
	write summary;

	-- world = get_initial_state_from_gui();

	while true do
		world = next_state();
		-- put_next_state_to_gui();
	endwhile;

	return 0;
end game_of_life

