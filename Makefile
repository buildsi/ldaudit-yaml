all:
	g++ -fPIC -shared -O3 -g -o auditlib.so auditlib.cpp
	
run:
	LD_AUDIT=./auditlib.so whoami
