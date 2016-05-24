all:
	 g++ -o Interpolate -g -std=c++14 Interpolate_test.cpp -Wall -Werror -pedantic
	 g++ -std=c++14 Function_test.cpp -o Function -g -Wall -Werror -pedantic
	 g++ -o SharedPtr -g -std=c++14 SharedPtr_test.cpp -pthread -Wall -Werror -pedantic