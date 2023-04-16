main: particleV2.h main.cpp color.h
	g++ "main.cpp" -lsfml-audio -lsfml-window -lsfml-graphics -lsfml-system -o main -Wall