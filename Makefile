CC := g++
SRCDIR := src
TESTDIR := tests
BUILDDIR := build
LOGDIR := logs
TARGET := bin/runner

SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)%.o, $(BUILDDIR)%.o, $(patsubst %.$(SRCEXT), %.o, $(SOURCES)))

CFLAGS := -Wall # -Wall
LIB := -L/usr/local/lib -lPocoUtil -lPocoNet -lPocoXML -lPocoJSON -lPocoFoundation
GTEST_LIBS := /usr/lib/libgtest.a /usr/lib/libgtest_main.a -lpthread
INC := -I include
TESTS = $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))


$(TARGET): $(OBJECTS)
	@mkdir -p bin
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
	@mkdir -p bin
	@echo " Collecting and compile tests...";
	$(CC) $(CFLAGS) $(TESTS) $(INC) $(LIB) $(GTEST_LIBS) -o bin/tester


test: compile_tests
	@echo " Running tests..."
	bin/tester --gtest_output=xml:gtestresults.xml

start: bin/runner
	@mkdir -p $(LOGDIR)
	./bin/runner > $(LOGDIR)/main.log &

deploy:
	@echo " Installing googletest libraries... "
	sh scripts/install-googletest.sh
	@echo " Installing Poco libraries... "
	sh scripts/install-poco.sh
	export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib

