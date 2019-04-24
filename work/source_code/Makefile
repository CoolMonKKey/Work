all:cut_csv cut_video gps

cut_csv:cut_csv.cpp
	g++ -o cut_csv cut_csv.cpp

cut_video:cut_video.cpp
	g++ -std=c++11 -o cut_video cut_video.cpp 

gps:gps.cpp
	g++ -std=c++11 -o gps gps.cpp

clean:
	rm cut_csv
	rm cut_video



