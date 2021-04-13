gcc_options = -std=c++17 -Wall -O2 --pedantic-errors

apparent_sun_moon: apparent_sun_moon.o apos.o jpl.o time.o delta_t.o file.o bpn.o obliquity.o convert.o matrix.o nutation.o
	g++102 $(gcc_options) -o $@ $^

apparent_sun_moon.o : apparent_sun_moon.cpp
	g++102 $(gcc_options) -c $<

apos.o : apos.cpp
	g++102 $(gcc_options) -c $<

jpl.o : jpl.cpp
	g++102 $(gcc_options) -c $<

time.o : time.cpp
	g++102 $(gcc_options) -c $<

delta_t.o : delta_t.cpp
	g++102 $(gcc_options) -c $<

file.o : file.cpp
	g++102 $(gcc_options) -c $<

bpn.o : bpn.cpp
	g++102 $(gcc_options) -c $<

obliquity.o : obliquity.cpp
	g++102 $(gcc_options) -c $<

convert.o : convert.cpp
	g++102 $(gcc_options) -c $<

matrix.o : matrix.cpp
	g++102 $(gcc_options) -c $<

nutation.o : nutation.cpp
	g++102 $(gcc_options) -c $<

run : apparent_sun_moon
	./apparent_sun_moon

clean :
	rm -f ./apparent_sun_moon
	rm -f ./*.o

.PHONY : run clean

