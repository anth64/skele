@echo off
if "%1"=="run" (
	if "%2"=="" (
		make -f gmake.mk run MODE=debug
	) else (
		make -f gmake.mk run MODE=%2
	)
) else (
	make -f gmake.mk %*
)
