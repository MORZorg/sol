func game_of_life() : int

	type	lines: vector [ 10 ] of bool;
			grid: vector [ 10 ] of lines;
			-- Just a bad idea.
			-- grid: struct( rows: lines; columns: lines; );

	var		world: struct( generation: int; world: grid; );
			filename: string;
			generations: int;
			initial_state: grid;

	const	world_size: int = 10;
			summary: string = "Welcome to ORZ's Conway's Game of Life!";
			enter_filename: string = "Enter the filename of your world.";
			enter_generations: string = "Enter for how many generations would you like to watch your world go by.";
			enter_world: string = "Your world doesn't exist yet.\nEnter it now.";

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

	write enter_filename;
	read filename;
	read [ filename ] world;

	write enter_generations;
	read generations;

	if world.generation == 0 then
		write enter_world;
		read initial_state;
		world.world = initial_state;
	endif;

	while generations > 0 do
		world.world = next_state( world.world );
        world.generation = world.generation + 1;
		write world;

		generations = generations - 1;
	endwhile;

	write [ filename ] world;

	return 0;
end game_of_life

