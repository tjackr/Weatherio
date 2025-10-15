# ======================================
# 🌤 Weatherio Main Makefile
# ======================================

MODULES := server client client_cpp

INCLUDES_DIR := includes
SERVER_INCLUDES := jansson tinydir.h md5.h md5.c
CLIENT_INCLUDES := jansson md5.h md5.c
CLIENT_CPP_INCLUDES := jansson

.PHONY: all \
	clean \
	$(MODULES) \
	$(addsuffix /addlinks,$(MODULES)) \
	$(addsuffix /rmlinks,$(MODULES)) \
	$(addsuffix /run,$(MODULES)) \
	$(addsuffix /clean,$(MODULES)) \
	$(addsuffix /valgrind,$(MODULES)) \
	create_symlinks \
	remove_symlinks

# Default target: build all modules
all: $(MODULES)

# Build each module with symlinks created first
$(MODULES):
	$(MAKE) create_symlinks MODULE=$@
	@echo "Building module $@..."
	$(MAKE) -C $@ all

# Create target symlinks using make [module]/addlinks
$(addsuffix /addlinks,$(MODULES)):
	@MODULE=$(@D); \
	$(MAKE) create_symlinks MODULE=$$MODULE;

# Remove target symlinks using make [module]/rmlinks
$(addsuffix /rmlinks,$(MODULES)):
	@MODULE=$(@D); \
	$(MAKE) remove_symlinks MODULE=$$MODULE;

# Run target using make [module]/run
$(addsuffix /run,$(MODULES)):
	@MODULE=$(@D); \
	$(MAKE) create_symlinks MODULE=$$MODULE; \
	echo "Running module $$MODULE..."; \
	$(MAKE) -C $$MODULE run

# Clean target using make [module]/clean
$(addsuffix /clean,$(MODULES)):
	@MODULE=$(@D); \
	echo "Cleaning module $$MODULE..."; \
	$(MAKE) remove_symlinks MODULE=$$MODULE; \
	$(MAKE) -C $$MODULE clean

# Run valgrind on target using [module]/valgrind
$(addsuffix /valgrind,$(MODULES)):
	@MODULE=$(@D); \
	$(MAKE) create_symlinks MODULE=$$MODULE; \
	echo "Valgrinding module $$MODULE..."; \
	$(MAKE) -C $$MODULE valgrind

#############################
# Symlink creation
#############################

# These use ln -s to link /includes files to [module]/src/includes/ dir.
# Remember if structure changes to also update paths here

create_symlinks:
	@INCLUDES_VAR=$(shell echo $(MODULE)_INCLUDES | tr a-z A-Z); \
	FILES=$$($(MAKE) -s -f $(lastword $(MAKEFILE_LIST)) print_includes VAR=$$INCLUDES_VAR); \
	for file in $$FILES; do \
		src="$(INCLUDES_DIR)/$$file"; \
		dst="$(MODULE)/src/includes/$$file"; \
		if [ ! -e "$$dst" ]; then \
			mkdir -p $$(dirname $$dst); \
			ln -s ../../../$$src $$dst; \
			echo "  Symlinked $$dst -> ../../../$$src"; \
		fi; \
	done

remove_symlinks:
	@INCLUDES_VAR=$(shell echo $(MODULE)_INCLUDES | tr a-z A-Z); \
	FILES=$$($(MAKE) -s -f $(lastword $(MAKEFILE_LIST)) print_includes VAR=$$INCLUDES_VAR); \
	for file in $$FILES; do \
		dst="$(MODULE)/src/includes/$$file"; \
		if [ -L "$$dst" ]; then \
			rm $$dst; \
			echo "  Removed symlink $$dst"; \
		fi; \
	done

print_includes:
	@echo $($(VAR))
