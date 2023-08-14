CC = gcc
RM = rm -f

all: clean cogsnet-prepare cogsnet-compute

cogsnet-prepare:
	$(CC) -o cogsnet/cogsnet-prepare cogsnet/cogsnet-prepare.c

cogsnet-compute:
	$(CC) -o cogsnet/cogsnet-compute cogsnet/cogsnet-compute.c -lm

clean:
	$(RM) cogsnet/cogsnet-compute cogsnet/cogsnet-prepare cogsnet/cogsnet-compute.sh
