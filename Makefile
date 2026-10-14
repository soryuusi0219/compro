ROOT_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

PYTHON ?= python3
CXX ?= g++
LIBRARY_MANAGER := $(PYTHON) $(ROOT_DIR)/tools/library_manager.py
VERIFY_TOOL := $(PYTHON) $(ROOT_DIR)/tools/verify.py
CONTEST_TOOL := $(PYTHON) $(ROOT_DIR)/tools/create_contest.py
EXPAND_TOOL := $(PYTHON) $(ROOT_DIR)/tools/expand_include.py

CXXFLAGS ?= -std=c++20 -O2 -Wall -Wextra
CXXFLAGS += -I$(ROOT_DIR) -I$(ROOT_DIR)/third_party/ac-library
LOCALFLAGS ?= -D_DEBUG

GOAL_CPP := $(filter %.cpp,$(MAKECMDGOALS))
SRC ?= $(or $(FILE),$(firstword $(GOAL_CPP)),$(CURDIR)/main.cpp)
SRC_PATH := $(abspath $(SRC))
SRC_DIR := $(dir $(SRC_PATH))
SRC_BASE := $(basename $(notdir $(SRC_PATH)))
OUT ?= $(SRC_DIR)a.out
SUBMIT ?= $(SRC_DIR)submit_$(SRC_BASE).cpp
EXPAND_FLAGS ?= --release --no-comments --strip-comments --strip-blank-lines

HEADERS ?=
TARGETS ?=
LIBRARY_ARGS ?=
VERIFY_ARGS ?=
CONTEST_ARGS ?=
BASE ?= origin/main
HEAD ?= HEAD
FORMAT ?= names
WORK_DIR ?= $(ROOT_DIR)/work
CONTEST_CONFIG ?= $(ROOT_DIR)/contest.json
# Do not inherit the commonly defined shell variable NAME; require NAME=... on CLI.
NAME :=
PROBLEMS :=
PROBLEM_COUNT :=
ACL :=
TEMPLATE :=
TESTER_TEMPLATE :=

.DEFAULT_GOAL := help

.PHONY: help build local run expand expand-library library-manager check
.PHONY: expand-verify report all affected verify verify-list contest create-contest
.PHONY: $(GOAL_CPP)

help:
	@echo "Library tools:"
	@echo "  make library-manager LIBRARY_ARGS='check|expand|...'"
	@echo "  make check"
	@echo "  make run [SRC=main.cpp|FILE=main.cpp|main.cpp]"
	@echo "  make expand [SRC=main.cpp|FILE=main.cpp|main.cpp]"
	@echo "  make expand-library [HEADERS='library/...'] [LIBRARY_ARGS='...']"
	@echo "  make expand-verify [TARGETS='target ...'] [LIBRARY_ARGS='...']"
	@echo "  make report [LIBRARY_ARGS='...']"
	@echo "  make all [LIBRARY_ARGS='...']"
	@echo "  make affected [BASE=origin/main] [HEAD=HEAD] [FORMAT=names|github]"
	@echo "  make verify [TARGETS='target ...'] [VERIFY_ARGS='...']"
	@echo "  make verify-list"
	@echo "  make contest NAME=abc123 [PROBLEM_COUNT=6] [ACL=true|false]"
	@echo "               [TEMPLATE=templates/custom.cpp] [PROBLEMS=ABCDEF]"
	@echo "  make create-contest CONTEST_ARGS='directory [options]'"

# Generic entry point for every library_manager.py subcommand.
library-manager:
	$(LIBRARY_MANAGER) $(LIBRARY_ARGS)

check:
	$(LIBRARY_MANAGER) check $(LIBRARY_ARGS)

build:
	$(CXX) $(CXXFLAGS) $(SRC_PATH) -o $(OUT)

local:
	$(CXX) $(CXXFLAGS) $(LOCALFLAGS) $(SRC_PATH) -o $(OUT)

run: local
	$(EXPAND_TOOL) $(SRC_PATH) -o $(SUBMIT) --root $(ROOT_DIR) $(EXPAND_FLAGS)
	$(OUT)

expand:
	$(EXPAND_TOOL) $(SRC_PATH) -o $(SUBMIT) --root $(ROOT_DIR) $(EXPAND_FLAGS)

expand-library:
	$(LIBRARY_MANAGER) expand $(HEADERS) $(LIBRARY_ARGS)

expand-verify:
	$(LIBRARY_MANAGER) expand-verify $(TARGETS) $(LIBRARY_ARGS)

report:
	$(LIBRARY_MANAGER) report $(LIBRARY_ARGS)

all:
	$(LIBRARY_MANAGER) all $(LIBRARY_ARGS)

affected:
	$(LIBRARY_MANAGER) affected --base $(BASE) --head $(HEAD) --format $(FORMAT) $(LIBRARY_ARGS)

verify:
	$(VERIFY_TOOL) $(TARGETS) $(VERIFY_ARGS)

verify-list:
	$(VERIFY_TOOL) --list

contest:
	@test -n "$(NAME)" || (echo "usage: make contest NAME=abc123 [PROBLEM_COUNT=12] [ACL=true|false]" >&2; exit 1)
	$(CONTEST_TOOL) $(WORK_DIR)/$(NAME) --root $(ROOT_DIR) --config $(CONTEST_CONFIG) \
		$(if $(PROBLEMS),--problems $(PROBLEMS)) \
		$(if $(PROBLEM_COUNT),--problem-count $(PROBLEM_COUNT)) \
		$(if $(ACL),--acl $(ACL)) \
		$(if $(TEMPLATE),--template $(TEMPLATE)) \
		$(if $(TESTER_TEMPLATE),--tester-template $(TESTER_TEMPLATE)) \
		$(CONTEST_ARGS)

# Generic entry point for create_contest.py, useful with a custom destination.
create-contest:
	$(CONTEST_TOOL) $(CONTEST_ARGS)

$(GOAL_CPP):
	@:
