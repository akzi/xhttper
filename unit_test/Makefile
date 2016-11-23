CFLAGS = -g -W \
-O3 \
-Wall \
-Wpointer-arith \
-Wshadow \
-Wno-long-long \
-Wformat \
-Winvalid-pch \
-std=c++11 \
-I ./../../include \
-l pthread
run:unit_test
	./unit_test
unit_test:rm_unit_test
	g++ unit_test.cpp $(CFLAGS) -o unit_test
rm_unit_test:
	$(shell if [ -f unit_test ]; then rm unit_test; fi;)

