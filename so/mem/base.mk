SRC_DIR = $(shell pwd)
OBJ_DIR = objs

SRCS = $(wildcard $(shell find $(SRC_DIR) -name "*.cpp"))
OBJS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))
DEPS = $(patsubst %.o, %.d, $(OBJS))

-include $(DEPS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@[ ! -e $(dir $@) ] &  mkdir -p $(dir $@)
	$(CXX) $(CFLAGS) -MF "$(@:%.o=%.d)" -MT "$@" -c $< -o $@

$(OUTDIR)/$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) 

all: $(OUTDIR)/$(TARGET)

clean:
	rm $(OBJ_DIR) -r

.PHONY: all clean
