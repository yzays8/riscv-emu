SRCDIR = src
FILE_PATH = $(abspath $(if $(filter /%,$(FILE)),$(FILE),$(dir $(lastword $(MAKEFILE_LIST)))$(FILE)))

.PHONY: all
all:
	make all -C $(SRCDIR)

.PHONY: clean
clean:
	make clean -C $(SRCDIR)

.PHONY: run
run:
	make run -C $(SRCDIR)

.PHONY: test
test:
	make test -C $(SRCDIR)
