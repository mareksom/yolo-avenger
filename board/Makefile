BUILD = build
CPP = clang++
CPPFLAGS = -std=c++11 `pkg-config gtkmm-3.0 --cflags`
LDFLAGS = `pkg-config gtkmm-3.0 --cflags --libs`
OBJS = main.o
HEADERS = HexBoard.h TriangleBoard.h Board.h Scene.h

all: $(BUILD)/run

$(BUILD)/run: $(addprefix $(BUILD)/, $(OBJS)) | $(BUILD)
	$(CPP) $^ -o $@ $(LDFLAGS)

$(BUILD)/%.o: %.cpp $(HEADERS) | $(BUILD)
	$(CPP) -c $< -o $@ $(CPPFLAGS)

$(BUILD):
	mkdir -p $(BUILD)

.PHONY: clean

clean:
	rm -rf $(BUILD)
