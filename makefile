# make all

PROG_NAME =

RM = cmd /c del /s /q
RMDIR = cmd /c rmdir /s /q
MKDIR = cmd /c mkdir
ECHO = cmd /c echo

SUFFIX = .exe
TARGET = $(PROG_NAME)$(SUFFIX)

CXX = g++
CXXFLAGS = -std=c++11 -Wall -c -g -DDEBUG
LDFLAGS =#-mwindows
# -Wl,--dynamicbase -Wl,--nxcompat -Wl,--high-entropy-va

SRC_DIR = source source/glad source/utils
LIB_DIR = lib
INC_DIR = include include/SDL2 include/glm include/utils
BIN_DIR = bin
TMP_DIR = tmp
LIB = winmm setupapi dxerr8 ole32 imm32 oleaut32 version gdi32 mingw32 SDL2main SDL2 SDL2_image assimp

CPPSOURCE = $(wildcard $(addsuffix /*.cpp, $(SRC_DIR)))
CSOURCE = $(wildcard $(addsuffix /*.c, $(SRC_DIR)))

INC = $(wildcard $(addsuffix /*.h, $(INC_DIR)))

all : init $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET) : $(patsubst %.cpp, $(TMP_DIR)/%.o, $(CPPSOURCE)) $(patsubst %.c, $(TMP_DIR)/%.o, $(CSOURCE))
	@$(ECHO) link $@
	@$(CXX) $^ $(addprefix -L, $(LIB_DIR)) $(addprefix -l, $(LIB)) $(LDFLAGS) -o $@

$(TMP_DIR)/%.o : %.c $(INC)
	@$(ECHO) compile $<
	@$(CXX) $(CXXFLAGS) $(addprefix -I, $(INC_DIR)) -o $@ $<

$(TMP_DIR)/%.o : %.cpp $(INC)
	@$(ECHO) compile $<
	@$(CXX) $(CXXFLAGS) $(addprefix -I, $(INC_DIR)) -o $@ $<

.PHONY : clean cleanall init all

clean :
	-@$(RMDIR) $(subst /,\,$(TMP_DIR))

cleanall : clean
	-@$(RM) $(subst /,\,$(BIN_DIR)/$(TARGET))

init :
	-@$(MKDIR) $(subst /,\,$(addprefix $(TMP_DIR)/, $(SRC_DIR)))
