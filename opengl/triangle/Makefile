CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra
LDLIBS   := -lGLEW -lglfw -lGL

TARGET   := triangle
SRCS     := main.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
