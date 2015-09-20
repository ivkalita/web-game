CC := g++
SRCDIR := src
TESTDIR := tests
BUILDDIR := build
TARGET := bin/runner

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)%.o, $(BUILDDIR)%.o, $(patsubst %.$(SRCEXT), %.o, $(SOURCES)))
<<<<<<< HEAD
CFLAGS := -g # -Wall
LIB := -L./lib/poco/lib -lPocoNet -lPocoUtil -lPocoFoundation
INC := -I./lib/poco/include
=======
CFLAGS := -Wall # -Wall
LIB :=
GTEST_LIBS := -L lib/gtest -l gtest_main -l gtest
INC := -I include
TESTS = $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))
>>>>>>> 4763196553520cf1197a393d948f2d7d578eb529

$(TARGET): $(OBJECTS)
	@echo " Linking..."
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	@echo " Cleaning...";
	@echo " $(RM) -r $(BUILDDIR) $(TARGET)"; $(RM) -r $(BUILDDIR) $(TARGET)

# Tests
compile_tests:
	@echo " Collecting and compile tests...";
	$(CC) $(CFLAGS) $(TESTS) $(INC) $(LIB) $(GTEST_LIBS) -o bin/tester


test: compile_tests
	@echo " Running tests..."
	bin/tester