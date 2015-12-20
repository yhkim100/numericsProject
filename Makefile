numerics: numerics.cpp numerics.h tree.cpp tree.h integrate.cpp integrate.h monteCarlo.cpp monteCarlo.h gradient.h gradient.cpp
	g++ -o numerics -std=gnu++98 -Wall -Werror -ggdb3 numerics.cpp tree.cpp integrate.cpp monteCarlo.cpp gradient.cpp

clean:
	rm -f *~ numerics
