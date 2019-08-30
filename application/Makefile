# Antiflood Copyright (C) 2019 Alexey <Inverse> Shumeiko

# This file is part of Antiflood project.

# This firmware is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# This firmware is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with the firmware. If not, see <http://www.gnu.org/licenses/>.

# For more details see LICENSE file.



.PHONY: all

all: help

################################################################
# During development you can redefine some behaviour
# in Makefile.config.

# Available for customization:
# (path value should end up with a slash-sign)
#   TEMP_DIR - build dir prefix (for example: /tmp/)

ifneq '' '$(wildcard Makefile.config)'
  include Makefile.config
else

endif

################################################################
# Determine platform/environment.

ifeq '' '$(TARGET)'
    TARGET := linux
else
  ifneq 'linux' '$(TARGET)'
    $(error Unsupported TARGET='$(TARGET)')
  endif
endif

################################################################
# Build type.
ifneq '' '$(filter %-release,$(MAKECMDGOALS))'
ifneq '' '$(filter %-debug,$(MAKECMDGOALS))'
$(error Do not mess up with build types.)
endif
endif

BUILD_TYPE := default
ifneq '' '$(filter %-debug,$(MAKECMDGOALS))'
BUILD_TYPE := debug
endif
ifneq '' '$(filter %-release,$(MAKECMDGOALS))'
BUILD_TYPE := release
endif

################################################################
# Extended goals.

$(filter clean-%,$(MAKECMDGOALS)):     clean
$(filter distclean-%,$(MAKECMDGOALS)): distclean
$(filter build-%,$(MAKECMDGOALS)):     build
$(filter rebuild-%,$(MAKECMDGOALS)):   rebuild
$(filter dist-%,$(MAKECMDGOALS)):      dist
$(filter run-%,$(MAKECMDGOALS)):       run

################################################################
# Base directories.

BUILD_DIR         ?= $(TEMP_DIR)build
DIST_DIR          ?= $(TEMP_DIR)dist
SRC_DIR            = src/hs

TARGET_BUILD_DIR   = $(BUILD_DIR)/$(TARGET)/$(BUILD_TYPE)
TARGET_DIST_DIR    = $(DIST_DIR)/$(TARGET)/$(BUILD_TYPE)

################################################################
# Source and object files.

SRCS       := $(wildcard $(SRC_DIR)/*.hs) $(wildcard $(SRC_DIR)/**/*.hs)
TESTER     := $(TARGET_DIST_DIR)/tester

################################################################
# Default goals.

.PHONY: clean distclean rebuild build dist

clean:
	rm -rf $(TARGET_BUILD_DIR)
	rm -rf $(TARGET_DIST_DIR)

distclean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

rebuild: clean dist

build: $(SRC_OBJS)

dist: $(SRC_OBJS)

################################################################
# Auto directories.

DIRS := $(dir $(OBJS))\
        $(TARGET_BUILD_DIR)\
        $(TARGET_DIST_DIR)

DIRS := $(patsubst %/,%,$(DIRS))
DIRS := $(patsubst %,%/,$(DIRS))
DIRS := $(sort $(DIRS))

$(DIRS):
	mkdir -p $@

################################################################
# Compilation and building.

.SECONDEXPANSION:

$(TESTER): $(SRCS) | $$(dir $$@) $$(TARGET_BUILD_DIR)
	ghc -o $@ -odir $(TARGET_BUILD_DIR) -hidir $(TARGET_BUILD_DIR) $^

################################################################
# Run something.

run: $(TESTER)
	$(TESTER)

################################################################
# Help printout.

help:
	@echo "Antiflood project"
	@echo
	@echo "Makefile's goals:"
	@echo "  help        prints this help"
	@echo "  run         executes the application"
	@echo
	@echo "Also 'Makefile.config' is possible."
	@echo "Define the following options (all path values should end up with a slash-sign):"
	@echo "  TEMP_DIR    build directory prefix (for example: /tmp/)"
	@echo
	@echo "Visit README for details:"
	@echo "> cat README"
	@echo

################################################################
# Debug the Makefile.

report_value = @echo $(1): '$($(1))'

.PHONY: debug

debug:
	$(call report_value,TEMP_DIR)
	$(call report_value,DIRS)
