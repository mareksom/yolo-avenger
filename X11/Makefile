BUILD = build
CPP = clang++
CPPFLAGS = -std=c++11
LDFLAGS = -lX11
OBJS = X.o PainterColors.o Key.o Mouse.o
GENFILES = Key.cpp Key.h PainterColors.cpp PainterColors.h
HEADERS = X.h Key.h PainterColors.h Mouse.h

BINARY_NAME = run
LIBRARY_NAME = Yolo11

LIBRARY_FILE_NAME = lib$(LIBRARY_NAME).a

all: lib

.PHONY: lib
lib: $(BUILD)/$(LIBRARY_FILE_NAME)

.PHONY: bin
bin: $(BUILD)/$(BINARY_NAME) 

# Key generation
Key.cpp Key.h: $(BUILD)/KeyGenerator.e Key/KeyGenerator.list
	$(BUILD)/KeyGenerator.e Key/KeyGenerator.list Key.cpp Key.h
$(BUILD)/KeyGenerator.e: Key/KeyGenerator.cpp | $(BUILD)
	$(CPP) $^ -o $@ $(CPPFLAGS) -lboost_regex

# PainterColors generation
PainterColors.cpp PainterColors.h: $(BUILD)/PainterColors.e PainterColors/PainterColorsGenerator.rgb.txt
	$(BUILD)/PainterColors.e PainterColors/PainterColorsGenerator.rgb.txt PainterColors.cpp PainterColors.h
$(BUILD)/PainterColors.e: PainterColors/PainterColorsGenerator.cpp | $(BUILD)
	$(CPP) $^ -o $@ $(CPPFLAGS) -lboost_regex

$(BUILD)/$(LIBRARY_FILE_NAME): $(addprefix $(BUILD)/, $(OBJS)) | $(BUILD)
	ar rcs $@ $^

$(BUILD)/run: $(BUILD)/main.o $(BUILD)/$(LIBRARY_FILE_NAME) | $(BUILD)
	$(CPP) $^ -o $@ $(LDFLAGS) -L$(BUILD) -l$(LIBRARY_NAME)

$(BUILD)/%.o: %.cpp $(HEADERS) | $(BUILD)
	$(CPP) -c $< -o $@ $(CPPFLAGS)

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	rm -rf $(BUILD) $(GENFILES)
