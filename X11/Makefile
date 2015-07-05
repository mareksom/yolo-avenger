BUILD = build
CPP = clang++
CPPFLAGS = -std=c++11
LDFLAGS = -lX11
OBJS = main.o X.o PainterColors.o Key.o Mouse.o
GENFILES = Key.cpp Key.h PainterColors.cpp PainterColors.h
HEADERS = X.h Key.h PainterColors.h Mouse.h

all: $(BUILD)/run

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

$(BUILD)/run: $(addprefix $(BUILD)/, $(OBJS)) | $(BUILD)
	$(CPP) $^ -o $@ $(LDFLAGS)

$(BUILD)/%.o: %.cpp $(HEADERS) | $(BUILD)
	$(CPP) -c $< -o $@ $(CPPFLAGS)

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean
clean:
	rm -rf $(BUILD) $(GENFILES)
