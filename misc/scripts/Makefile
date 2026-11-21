ATCODER_ROOT := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
COMPRO_ROOT := $(ATCODER_ROOT)/git/compro
PYTHON ?= python3
EXPAND_TOOL := $(PYTHON) $(COMPRO_ROOT)/tools/expand_include.py

NAME :=
PROBLEM_COUNT :=

GOAL_CPP := $(filter %.cpp,$(MAKECMDGOALS))
SRC ?= $(or $(FILE),$(firstword $(GOAL_CPP)),$(CURDIR)/main.cpp)
SRC_PATH := $(abspath $(SRC))
SRC_DIR := $(dir $(SRC_PATH))
SRC_BASE := $(basename $(notdir $(SRC_PATH)))
SUBMIT ?= $(SRC_DIR)submit_$(SRC_BASE).cpp
EXPAND_FLAGS ?= --release --no-comments --strip-comments --strip-blank-lines

.DEFAULT_GOAL := help

.PHONY: help expand abcsetup arcsetup agcsetup cfsetup recommendation_setup
.PHONY: $(GOAL_CPP)

help:
	@echo "Source tools:"
	@echo "  make expand [SRC=path/to/a.cpp|FILE=path/to/a.cpp|path/to/a.cpp]"
	@echo ""
	@echo "Contest setup:"
	@echo "  make abcsetup NAME=462 [PROBLEM_COUNT=7]"
	@echo "  make arcsetup NAME=233 [PROBLEM_COUNT=6]"
	@echo "  make agcsetup NAME=079 [PROBLEM_COUNT=6]"
	@echo "  make cfsetup NAME=Educational_193 [PROBLEM_COUNT=14]"
	@echo "  make recommendation_setup NAME=ABC123_D"

expand:
	$(EXPAND_TOOL) $(SRC_PATH) -o $(SUBMIT) --root $(COMPRO_ROOT) $(EXPAND_FLAGS)

abcsetup arcsetup agcsetup cfsetup:
	@test -n "$(NAME)" || (echo "usage: make $@ NAME=... [PROBLEM_COUNT=...]" >&2; exit 1)
	@$(ATCODER_ROOT)/$@.sh "$(NAME)" $(if $(strip $(PROBLEM_COUNT)),"$(PROBLEM_COUNT)")

recommendation_setup:
	@test -n "$(NAME)" || (echo "usage: make $@ NAME=..." >&2; exit 1)
	@$(ATCODER_ROOT)/recommendation_setup.sh "$(NAME)"

$(GOAL_CPP):
	@:
