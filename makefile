.default: all

all: DiningPhilosopher_Simulation Producer_Consumer_Simulation

clean:
	rm -f DiningPhilosopher_Simulation *.o
	rm -f Producer_Consumer_Simulation *.o
Producer_Consumer_Simulation : Producer_Consumer_Simulation.o
	g++ -pthread -Wall -Werror -o $@ $^

DiningPhilosopher_Simulation : DiningPhilosopher_Simulation.o
	g++ -pthread -Wall -Werror -o $@ $^

%.o: %.c
	g++ -pthread -Wall -Werror -c++ $^

