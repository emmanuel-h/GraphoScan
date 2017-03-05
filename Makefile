# Handle environment variables
ifeq ($(wildcard ${OPT_INC}),)
	CXX   = g++
	ODIR  = .obj/build${D}
	SDIR  = .
	MKDIR = mkdir -p
endif

BASE_NAME = acquisition
NAME      = ${BASE_NAME}${D}
INC       = -I/usr/include/flycapture
LIBS      = -lflycapture${D} ${FC2_DEPS}

_OBJ      = ${BASE_NAME}.o
SRC       = $(_OBJ:.o=.cpp)
OBJ       = $(patsubst %,$(ODIR)/%,$(_OBJ))

OPENCV	= `pkg-config --cflags --libs opencv`

# Master rule
.PHONY: all
all: ${NAME}

# Output binary
${NAME}: ${OBJ}
	${CXX} ${OBJ} -o ${NAME} ${LIBS} ${OPENCV} -O2

# Intermediate object files
${OBJ}: ${ODIR}/%.o : ${SDIR}/%.cpp
	@${MKDIR} ${ODIR}
	${CXX} ${CXXFLAGS} ${LINUX_DEFINES} ${INC} -Wall -c $< -o $@
