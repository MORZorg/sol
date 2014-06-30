func game_of_life() : int

	type	lines: vector [ 10 ] of bool;
			grid: vector [ 10 ] of lines;
			-- Just a bad idea.
			-- grid: struct( rows: lines; columns: lines; );

	var		world: struct( name: string; generation: int; world: grid; );
			control: struct( continue, edit: bool; );

	const	summary: string = "Welcome to ORZ's Conway's Game of Life!";
			world_size: int = 10;

	-- Rules:
	--   Any live cell with fewer than two live neighbours dies, as if caused by under-population.
	--   Any live cell with two or three live neighbours lives on to the next generation.
	--   Any live cell with more than three live neighbours dies, as if by overcrowding.
	--   Any dead cell with exactly three live neighbours becomes a live cell, as if by reproduction.
	func next_state( current_state: grid; ) : grid
		var		i, j, k, h: int;
				neighbours: int;
				state: grid;

	begin next_state

		for i = 0 to world_size do
			for j = 0 to world_size do

				neighbours = 0;
				for k = ( if i-1>0 then i-1 else 0 endif ) to ( if i+1<world_size then i+1 else world_size endif ) do
					for h = ( if j-1>0 then j-1 else 0 endif ) to ( if j+1<world_size then j+1 else world_size endif ) do
						if current_state[ k ][ h ] then
							neighbours = neighbours + 1;
						endif;
					endfor;
				endfor;

				state[ i ][ j ] = if state[ i ][ j ]
					then neighbours >= 2 and neighbours <= 3
					else neighbours == 3 endif;
			endfor;
		endfor;

		return state;
	end next_state

begin game_of_life
	write summary;

	read world;
	control = struct( true, false );

	while control.continue do
		world.world = next_state( world.world );
        world.generation = world.generation + 1;
		write world;

		read control;
		if control.edit then
			read world;
		endif;
	endwhile;

	write [ world.name ] world;

	return 0;
end game_of_life

