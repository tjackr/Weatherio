# ======================================
# 🌤 Weatherio Main Makefile 
# For linux (and maybe macos)
# ======================================

MODULES := server client client_cpp


#############################
# Libs assignment
#############################

# Define which libs to link to which module using symlink functions


LIBS_DIR := libs

SERVER_LIBS := utils/linkedlist.h \
							 utils/linkedlist.c \
							 includes/jansson \
							 includes/tinydir.h \
							 includes/md5.h \
							 includes/md5.c

CLIENT_LIBS := cJSON.c \
							 cJSON.h \
							 includes/tinydir.h \
							 includes/md5.h \
							 includes/md5.c

CLIENT_CPP_LIBS := includes/jansson


#############################
# PHONIES
#############################


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


#############################
# Recipes
#############################


# Default target: build all modules
all: $(MODULES)

clean:
	@for module in $(MODULES); do \
		echo "Cleaning $$module..."; \
		$(MAKE) remove_symlinks MODULE=$$module; \
		$(MAKE) -C $$module clean; \
	done
	@echo "All modules cleaned."

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


# This could probably be improved to dynamically find how many steps back source is
# i.e not use ../../../../ statically
create_symlinks:
	@LIBS_VAR=$(shell echo $(MODULE)_LIBS | tr a-z A-Z); \
	FILES=$$($(MAKE) -s -f $(lastword $(MAKEFILE_LIST)) print_includes VAR=$$LIBS_VAR); \
	for file in $$FILES; do \
		src="$(LIBS_DIR)/$$file"; \
		dst="$(MODULE)/src/libs/$$file"; \
		if [ ! -e "$$dst" ]; then \
			mkdir -p $$(dirname $$dst); \
			ln -s ../../../../$$src $$dst; \
			echo "  Symlinked $$dst -> ../../../../$$src"; \
		fi; \
	done

remove_symlinks:
	@LIBS_VAR=$(shell echo $(MODULE)_LIBS | tr a-z A-Z); \
	FILES=$$($(MAKE) -s -f $(lastword $(MAKEFILE_LIST)) print_includes VAR=$$LIBS_VAR); \
	for file in $$FILES; do \
		dst="$(MODULE)/src/libs/$$file"; \
		if [ -L "$$dst" ]; then \
			rm $$dst; \
			echo "  Removed symlink $$dst"; \
		fi; \
	done

print_includes:
	@echo $($(VAR))
