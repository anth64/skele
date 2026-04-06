CLOCK_SRC = src/platform/clock/posix.c
INPUT_SRC = src/platform/client/input/sdl.c

.include "config.mk"

ENGINE_SRCS      = src/skele.c ${CLOCK_SRC}
SOFT_PLAT_SRCS   = src/platform/client/video/sdl.c ${INPUT_SRC}
GL_PLAT_SRCS     = src/platform/client/video/gl.c ${INPUT_SRC}
SOFT_CLIENT_SRCS = example/client/main.c
GL_CLIENT_SRCS   = example/gl_client/main.c
SERVER_EXAMPLE   = example/server/main.c

LIBSKELE_SRCS     = ${ENGINE_SRCS} ${SOFT_PLAT_SRCS}
LIBGLSKELE_SRCS   = ${ENGINE_SRCS} ${GL_PLAT_SRCS}
LIBSKELE_SRV_SRCS = ${ENGINE_SRCS}

LIBSKELE     = ${BIN_DIR}/debug/libskele.a
LIBGLSKELE   = ${BIN_DIR}/debug/libglskele.a
LIBSKELE_SRV = ${BIN_DIR}/debug/libskeleserver.a
SKELE_BIN    = ${BIN_DIR}/debug/skele_client
GLSKELE_BIN  = ${BIN_DIR}/debug/glskele_client
SERVER_BIN   = ${BIN_DIR}/debug/skele_server

LIBSKELE_R     = ${BIN_DIR}/release/libskele.a
LIBGLSKELE_R   = ${BIN_DIR}/release/libglskele.a
LIBSKELE_SRV_R = ${BIN_DIR}/release/libskeleserver.a
SKELE_BIN_R    = ${BIN_DIR}/release/skele_client
GLSKELE_BIN_R  = ${BIN_DIR}/release/glskele_client
SERVER_BIN_R   = ${BIN_DIR}/release/skele_server

LDFLAGS_PLAT = -L/usr/local/lib
CFLAGS_PLAT  = -I/usr/local/include
CFLAGS_BASE  = -Wall -Wpedantic -I${.CURDIR}/${INC_DIR} -std=c99 ${CFLAGS_PLAT} ${DEBUG_FLAGS} \
               -DGL_MAJOR=${GL_MAJOR} -DGL_MINOR=${GL_MINOR}
LINK_STK     = -Wl,-Bstatic -lstk -Wl,-Bdynamic

LIBSKELE_DEBUG_OBJS     = ${LIBSKELE_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/debug\/soft\//}
LIBGLSKELE_DEBUG_OBJS   = ${LIBGLSKELE_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/debug\/gl\//}
LIBSKELE_SRV_DEBUG_OBJS = ${LIBSKELE_SRV_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/debug\/server\//}
SOFT_CLIENT_DEBUG_OBJS  = ${SOFT_CLIENT_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/debug\/soft\//}
GL_CLIENT_DEBUG_OBJS    = ${GL_CLIENT_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/debug\/gl\//}
SERVER_DEBUG_OBJS       = ${SERVER_EXAMPLE:S/.c$/.o/:S/^/${.CURDIR}\/obj\/debug\/server\//}

LIBSKELE_RELEASE_OBJS     = ${LIBSKELE_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/release\/soft\//}
LIBGLSKELE_RELEASE_OBJS   = ${LIBGLSKELE_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/release\/gl\//}
LIBSKELE_SRV_RELEASE_OBJS = ${LIBSKELE_SRV_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/release\/server\//}
SOFT_CLIENT_RELEASE_OBJS  = ${SOFT_CLIENT_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/release\/soft\//}
GL_CLIENT_RELEASE_OBJS    = ${GL_CLIENT_SRCS:S/.c$/.o/:S/^/${.CURDIR}\/obj\/release\/gl\//}
SERVER_RELEASE_OBJS       = ${SERVER_EXAMPLE:S/.c$/.o/:S/^/${.CURDIR}\/obj\/release\/server\//}

ALL_SOFT_SRCS   = ${LIBSKELE_SRCS} ${SOFT_CLIENT_SRCS}
ALL_GL_SRCS     = ${LIBGLSKELE_SRCS} ${GL_CLIENT_SRCS}
ALL_SERVER_SRCS = ${LIBSKELE_SRV_SRCS} ${SERVER_EXAMPLE}

PREFIX   ?= /usr/local
GL_MAJOR ?= 3
GL_MINOR ?= 3

.PHONY: all debug release skele glskele server libs install uninstall clean run run_gl

all: debug

debug: ${.CURDIR}/${LIBSKELE} ${.CURDIR}/${LIBGLSKELE} ${.CURDIR}/${LIBSKELE_SRV} \
       ${.CURDIR}/${SKELE_BIN} ${.CURDIR}/${GLSKELE_BIN} ${.CURDIR}/${SERVER_BIN}

release: ${.CURDIR}/${LIBSKELE_R} ${.CURDIR}/${LIBGLSKELE_R} ${.CURDIR}/${LIBSKELE_SRV_R} \
         ${.CURDIR}/${SKELE_BIN_R} ${.CURDIR}/${GLSKELE_BIN_R} ${.CURDIR}/${SERVER_BIN_R}

libs:    ${.CURDIR}/${LIBSKELE} ${.CURDIR}/${LIBGLSKELE} ${.CURDIR}/${LIBSKELE_SRV}
skele:   ${.CURDIR}/${LIBSKELE} ${.CURDIR}/${SKELE_BIN}
glskele: ${.CURDIR}/${LIBGLSKELE} ${.CURDIR}/${GLSKELE_BIN}
server:  ${.CURDIR}/${LIBSKELE_SRV} ${.CURDIR}/${SERVER_BIN}

run:    skele
	${.CURDIR}/${SKELE_BIN}

run_gl: glskele
	${.CURDIR}/${GLSKELE_BIN}

${.CURDIR}/${LIBSKELE}: ${LIBSKELE_DEBUG_OBJS}
	@mkdir -p ${.TARGET:H}
	ar rcs ${.TARGET} ${.ALLSRC}

${.CURDIR}/${LIBGLSKELE}: ${LIBGLSKELE_DEBUG_OBJS}
	@mkdir -p ${.TARGET:H}
	ar rcs ${.TARGET} ${.ALLSRC}

${.CURDIR}/${LIBSKELE_SRV}: ${LIBSKELE_SRV_DEBUG_OBJS}
	@mkdir -p ${.TARGET:H}
	ar rcs ${.TARGET} ${.ALLSRC}

${.CURDIR}/${LIBSKELE_R}: ${LIBSKELE_RELEASE_OBJS}
	@mkdir -p ${.TARGET:H}
	ar rcs ${.TARGET} ${.ALLSRC}

${.CURDIR}/${LIBGLSKELE_R}: ${LIBGLSKELE_RELEASE_OBJS}
	@mkdir -p ${.TARGET:H}
	ar rcs ${.TARGET} ${.ALLSRC}

${.CURDIR}/${LIBSKELE_SRV_R}: ${LIBSKELE_SRV_RELEASE_OBJS}
	@mkdir -p ${.TARGET:H}
	ar rcs ${.TARGET} ${.ALLSRC}

${.CURDIR}/${SKELE_BIN}: ${SOFT_CLIENT_DEBUG_OBJS} ${.CURDIR}/${LIBSKELE}
	@mkdir -p ${.TARGET:H}
	${CC} -o ${.TARGET} ${SOFT_CLIENT_DEBUG_OBJS} -L${.CURDIR}/${BIN_DIR}/debug -lskele ${LINK_STK} -lSDL3 ${LDFLAGS_PLAT}

${.CURDIR}/${GLSKELE_BIN}: ${GL_CLIENT_DEBUG_OBJS} ${.CURDIR}/${LIBGLSKELE}
	@mkdir -p ${.TARGET:H}
	${CC} -o ${.TARGET} ${GL_CLIENT_DEBUG_OBJS} -L${.CURDIR}/${BIN_DIR}/debug -lglskele ${LINK_STK} -lSDL3 -lGL ${LDFLAGS_PLAT}

${.CURDIR}/${SERVER_BIN}: ${SERVER_DEBUG_OBJS} ${.CURDIR}/${LIBSKELE_SRV}
	@mkdir -p ${.TARGET:H}
	${CC} -o ${.TARGET} ${SERVER_DEBUG_OBJS} -L${.CURDIR}/${BIN_DIR}/debug -lskeleserver ${LINK_STK} ${LDFLAGS_PLAT}

${.CURDIR}/${SKELE_BIN_R}: ${SOFT_CLIENT_RELEASE_OBJS} ${.CURDIR}/${LIBSKELE_R}
	@mkdir -p ${.TARGET:H}
	${CC} -s -o ${.TARGET} ${SOFT_CLIENT_RELEASE_OBJS} -L${.CURDIR}/${BIN_DIR}/release -lskele ${LINK_STK} -lSDL3 ${LDFLAGS_PLAT}

${.CURDIR}/${GLSKELE_BIN_R}: ${GL_CLIENT_RELEASE_OBJS} ${.CURDIR}/${LIBGLSKELE_R}
	@mkdir -p ${.TARGET:H}
	${CC} -s -o ${.TARGET} ${GL_CLIENT_RELEASE_OBJS} -L${.CURDIR}/${BIN_DIR}/release -lglskele ${LINK_STK} -lSDL3 -lGL ${LDFLAGS_PLAT}

${.CURDIR}/${SERVER_BIN_R}: ${SERVER_RELEASE_OBJS} ${.CURDIR}/${LIBSKELE_SRV_R}
	@mkdir -p ${.TARGET:H}
	${CC} -s -o ${.TARGET} ${SERVER_RELEASE_OBJS} -L${.CURDIR}/${BIN_DIR}/release -lskeleserver ${LINK_STK} ${LDFLAGS_PLAT}

.for _src in ${ALL_SOFT_SRCS}
${.CURDIR}/obj/debug/soft/${_src:S/.c$/.o/}: ${_src}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS_BASE} -g -O0 -MMD -MP -MT ${.TARGET} -MF ${.TARGET:S/.o$/.d/} -c ${.ALLSRC} -o ${.TARGET}
${.CURDIR}/obj/release/soft/${_src:S/.c$/.o/}: ${_src}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS_BASE} -O2 -MMD -MP -MT ${.TARGET} -MF ${.TARGET:S/.o$/.d/} -c ${.ALLSRC} -o ${.TARGET}
.endfor

.for _src in ${ALL_GL_SRCS}
${.CURDIR}/obj/debug/gl/${_src:S/.c$/.o/}: ${_src}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS_BASE} -g -O0 -MMD -MP -MT ${.TARGET} -MF ${.TARGET:S/.o$/.d/} -c ${.ALLSRC} -o ${.TARGET}
${.CURDIR}/obj/release/gl/${_src:S/.c$/.o/}: ${_src}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS_BASE} -O2 -MMD -MP -MT ${.TARGET} -MF ${.TARGET:S/.o$/.d/} -c ${.ALLSRC} -o ${.TARGET}
.endfor

.for _src in ${ALL_SERVER_SRCS}
${.CURDIR}/obj/debug/server/${_src:S/.c$/.o/}: ${_src}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS_BASE} -g -O0 -MMD -MP -MT ${.TARGET} -MF ${.TARGET:S/.o$/.d/} -c ${.ALLSRC} -o ${.TARGET}
${.CURDIR}/obj/release/server/${_src:S/.c$/.o/}: ${_src}
	@mkdir -p ${.TARGET:H}
	${CC} ${CFLAGS_BASE} -O2 -MMD -MP -MT ${.TARGET} -MF ${.TARGET:S/.o$/.d/} -c ${.ALLSRC} -o ${.TARGET}
.endfor

.-include "obj/debug/soft/*.d"
.-include "obj/debug/gl/*.d"
.-include "obj/debug/server/*.d"
.-include "obj/release/soft/*.d"
.-include "obj/release/gl/*.d"
.-include "obj/release/server/*.d"

install: release
	install -d ${PREFIX}/lib
	install -d ${PREFIX}/include/skele
	install -m 644 ${.CURDIR}/${LIBSKELE_R}     ${PREFIX}/lib/libskele.a
	install -m 644 ${.CURDIR}/${LIBGLSKELE_R}   ${PREFIX}/lib/libglskele.a
	install -m 644 ${.CURDIR}/${LIBSKELE_SRV_R} ${PREFIX}/lib/libskeleserver.a
	cp -r ${.CURDIR}/include/ ${PREFIX}/include/skele/
	install -d ${PREFIX}/bin

uninstall:
	rm -f ${PREFIX}/lib/libskele.a
	rm -f ${PREFIX}/lib/libglskele.a
	rm -f ${PREFIX}/lib/libskeleserver.a
	rm -rf ${PREFIX}/include/skele

clean:
	rm -rf ${.CURDIR}/${OBJ_DIR} ${.CURDIR}/${BIN_DIR}
