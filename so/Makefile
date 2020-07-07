CC = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar

EXTRA_CFLAGS = -g
EXTPA_LFLAGS = -L. -lwt-calibration
PROJECT = test


SRC  =	main.cpp

OBJS = $(SRC:.c=.o)
All: $(OBJS) $(PROJECT)


%o : %c
	@echo "Compiling $<"
	$(CC) -c $(EXTRA_CFLAGS) $< -o $@

$(PROJECT): $(OBJS)
	@echo "Linking $@"
	@$(CC) $(OBJS)  -lpthread -ldl -lrt -lm $(EXTPA_LFLAGS)  -o $@

clean :
	@echo "cleaning"
	rm -rf $(PROJECT) *.o
	
