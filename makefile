.PHONY: all clean install uninstall

all:
	cd lib && $(MAKE) $(MAKECMDGOALS)

clean:
	cd lib && $(MAKE) $(MAKECMDGOALS)

install:
	cd lib && $(MAKE) $(MAKECMDGOALS)

uninstall:
	cd lib && $(MAKE) $(MAKECMDGOALS)
