CC := g++
SRCDIR := src
TESTDIR := tests
BUILDDIR := build
TARGET := bin/runner

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)%.o, $(BUILDDIR)%.o, $(patsubst %.$(SRCEXT), %.o, $(SOURCES)))

CFLAGS := -Wall # -Wall
LIB := -L./lib/poco -lPocoNet -lPocoUtil -lPocoFoundation -lPocoXML -lPocoJSON
GTEST_LIBS := -L lib/gtest -l gtest_main -l gtest
INC := -I include
TESTS = $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))


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
