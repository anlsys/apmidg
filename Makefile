#
#
#

CFLAGS=-Wall -O2 -g -I.
LDFLAGS=-lze_loader -L. -lapmidg -lstdc++

SRCS=$(shell ls -1 *.c)
BINS=testlibapmidg libapmidg.so

all: $(BINS)

% : %.c
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

% : %.cpp
	$(CXX) -o $@ $< $(CFLAGS) $(LDFLAGS)

%.o : %.cpp
	$(CXX) -c $(CFLAGS) -fpic $<

libapmidg.so : libapmidg.o apmidg_zmacrostr.o
	$(CC) -shared -o $@ $^ -lze_loader -lstdc++

libapmidg.a : libapmidg.o apmidg_zmacrostr.o
	ar -crs $@ $^

testlibapmidg: testlibapmidg.c libapmidg.so libapmidg.h
	$(CC) -o $@ $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(BINS) *.o *.a
