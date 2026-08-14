CXX      := g++
CXXFLAGS := -std=c++23 -Wall -Wextra
LDLIBS   := -lGLEW -lglfw -lGL
INCLUDES := -I../../ -I../../general/glm -I../../general/stb

TARGET   := texture-mix
COMMON   := ../common
SRCS     := main.cpp $(COMMON)/OpenGLShaders.cpp $(COMMON)/Texture.cpp
OBJS     := $(notdir $(SRCS:.cpp=.o))
DEPS     := $(OBJS:.o=.d)

VPATH    := $(COMMON)

ifeq ($(PROFILE),1)
CXXFLAGS += -DPROFILER_ENABLE_INLINE_MACRO
endif

.PHONY: all run clean profile run-profile

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

run: $(TARGET)
	./$(TARGET)

profile:
	$(MAKE) clean
	$(MAKE) PROFILE=1 all

run-profile: profile
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)

-include $(DEPS)
