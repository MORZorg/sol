func hello_world(): int
	var
		s: string;
begin hello_world
	write [ "helloworld.sbra" ] "Hello, world!";
	read [ "helloworld.sbra" ] s;
	write s;
	return 0;
end hello_world
