# SimpleOS Kernel - Makefile

CC      = gcc
CXX     = g++
CFLAGS  = -std=c99  -Wall -Wextra -pedantic -g
CXXFLAGS= -std=c++14 -Wall -Wextra -g
LDFLAGS = -lpthread

.PHONY: all test temizle

all: test_kernel

kernelintegration.o: kernelintegration.c kernel.h
	$(CC) $(CFLAGS) -c $< -o $@

test_kernel: test_kernel.cpp kernelintegration.o KernelArayuz.hpp kernel.h
	$(CXX) $(CXXFLAGS) $< kernelintegration.o -o $@ $(LDFLAGS)

test: test_kernel
	./test_kernel

temizle:
	rm -f *.o test_kernel
