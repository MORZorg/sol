func game_of_life() : int

	type	lines: vector [ 15 ] of bool;
			grid: vector [ 15 ] of lines;
			-- Just a bad idea.
			-- grid: struct( rows: lines; columns: lines; );

	var		world: struct( generation: int; world: grid; );
			input: struct( filename: string; load: bool; );
			generations: int;
			initial_state: grid;

	const	world_size: int = 15;
			summary: string = "Welcome to ORZ's Conway's Game of Life!";
			enter_filename: string = "Enter the filename of your world and if you'd like to load from a saved state.";
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

		const	neighbour_offset: vector[ 3 ] of int = vector( -1, 0, 1 );

	begin next_state

		for i = 0 to world_size do
			for j = 0 to world_size do

				neighbours = 0;
				foreach k in neighbour_offset do
					foreach h in neighbour_offset do
						if k != 0 or h != 0 then
							if i+k >= 0 and i+k < world_size and j+h >= 0 and j+h < world_size then
								-- write struct( "Counting", vector( vector( i, j ), vector( k, h ) ), current_state[ i + k ][ j + h ] );
								if current_state[ i + k ][ j + h ] then
									neighbours = neighbours + 1;
								endif;
							endif;
						endif;
					endforeach;
				endforeach;

				state[ i ][ j ] = if current_state[ i ][ j ]
					then neighbours == 2 or neighbours == 3
					else neighbours == 3 endif;

				-- write struct( vector( i, j ), vector( current_state[ i ][ j ], state[ i ][ j ] ), neighbours );
			endfor;
		endfor;

		return state;
	end next_state

begin game_of_life
	write summary;

	write enter_filename;
	read input;
	if input.load then
		read [ input.filename ] world;
	else
		write enter_world;
		read initial_state;
		world.world = initial_state;
	endif;

	write enter_generations;
	read generations;

	while generations > 0 do
		world.world = next_state( world.world );
        world.generation = world.generation + 1;
		write world;

		generations = generations - 1;
	endwhile;

	write [ input.filename ] world;

	return 0;
end game_of_life

