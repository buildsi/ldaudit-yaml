all:
	g++ -fPIC -shared -O3 -g -o auditlib.so auditlib.cpp
	
run:
	LD_AUDIT=./auditlib.so whoami

dot:
	python generate_dot.py ldaudit.yaml > audit.dot
	dot -Tpng audit.dot -o audit.png
