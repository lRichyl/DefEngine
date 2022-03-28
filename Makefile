# Alternative GNU Make workspace makefile autogenerated by Premake

ifndef config
  config=debug
endif

ifndef verbose
  SILENT = @
endif

ifeq ($(config),debug)
  DefEngine_config = debug

else ifeq ($(config),release)
  DefEngine_config = release

else
  $(error "invalid configuration $(config)")
endif

PROJECTS := DefEngine

.PHONY: all clean help $(PROJECTS) 

all: $(PROJECTS)

DefEngine:
ifneq (,$(DefEngine_config))
	@echo "==== Building DefEngine ($(DefEngine_config)) ===="
	@${MAKE} --no-print-directory -C . -f DefEngine.make config=$(DefEngine_config)
endif

clean:
	@${MAKE} --no-print-directory -C . -f DefEngine.make clean

help:
	@echo "Usage: make [config=name] [target]"
	@echo ""
	@echo "CONFIGURATIONS:"
	@echo "  debug"
	@echo "  release"
	@echo ""
	@echo "TARGETS:"
	@echo "   all (default)"
	@echo "   clean"
	@echo "   DefEngine"
	@echo ""
	@echo "For more information, see https://github.com/premake/premake-core/wiki"