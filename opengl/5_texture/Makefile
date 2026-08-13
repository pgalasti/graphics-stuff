CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra
LDLIBS   := -lGLEW -lglfw -lGL
INCLUDES := -I../../ -I../../general/glm -I../../general/stb

TARGET   := texture
COMMON   := ../common
SRCS     := main.cpp $(COMMON)/OpenGLShaders.cpp $(COMMON)/Texture.cpp
OBJS     := $(notdir $(SRCS:.cpp=.o))
DEPS     := $(OBJS:.o=.d)

VPATH    := $(COMMON)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
