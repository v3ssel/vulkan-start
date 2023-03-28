CXX = g++
CONFIG = -O1 -std=c++17
CFLAGS = -Wall \
		 -Werror \
		 -Wextra

SOURCES = main.cpp \
		  VulkanManager/VulkanManager.cpp 	 \
		  VulkanValidator/VulkanValidator.cpp \
		  QueueFamilies/QueueFamilies.cpp  	   \
		  SwapChainDetails/SwapChainDetails.cpp \
		  DisplayWindow/DisplayWindow.cpp


DEFINES = -D NDEBUG

INCLUDE_DIRS = -I "C:\Coding\Vulkan\Include"
INCLUDE_LIBS = -L "C:\Coding\Vulkan\Lib"

LIBS = -lvulkan-1 \
	   -lglfw3 	  \
	   -lopengl32 \
	   -lgdi32

all: build run

build:
	$(CXX) $(CONFIG) $(SOURCES) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $(LIBS)

release:
	$(CXX) $(CONFIG) $(SOURCES) $(DEFINES) $(INCLUDE_DIRS) $(INCLUDE_LIBS) $(LIBS)

run:
	.\a.exe