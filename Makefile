PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

OBJS = radix_tree.o

BUILD_MODE = debug
ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else ifeq ($(BUILD_MODE),run)
	CFLAGS += -O2
else
	$(warning Build mode not $(BUILD_MODE) supported by this Makefile)
endif

all:	radix

radix:	$(OBJS)
	$(CXX) -o $@ $^


%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr radix $(OBJS)
