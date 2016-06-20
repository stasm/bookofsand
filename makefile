# Compiler
CFLAGS  ?= -std=c99
LDFLAGS ?= -lncurses

# Folders
OBJ_DIR = obj
SRC_DIR = src

# File lists
DISTRIB = spell
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

all: $(DISTRIB)

$(DISTRIB): $(OBJECTS)
	@echo "Linking executable" $(OBJECTS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -o $@ $+ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling" $@
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJ_DIR)
	rm -f  $(DISTRIB)

.PHONY: all clean
