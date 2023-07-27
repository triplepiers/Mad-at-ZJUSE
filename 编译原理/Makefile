TOP_DIR := $(PWD)
SRC_DIR := $(TOP_DIR)/src
TEST_DIR := $(TOP_DIR)/test
VISUAL_DIR := $(TOP_DIR)/visual

.PHONY: test all clean

all:
	@$(MAKE) -C $(SRC_DIR)

	@echo "building..."
	@mkdir -p build && cd ./build && cmake .. && make
	@echo "Successfully generate ./build/Compiler."

build:
	@echo "building..."
	@mkdir -p build && cd ./build && cmake .. && make
	@echo "Successfully generate ./build/Compiler."

test: 
	@$(MAKE) -C $(TEST_DIR) test

ll:
	@$(MAKE) -C $(TEST_DIR) ll

clean:
	@$(MAKE) -C $(SRC_DIR) clean
	@$(MAKE) -C $(TEST_DIR) clean
	@$(MAKE) -C $(VISUAL_DIR) clean

	@echo "Cleanning ./build ..."
	@rm -rf ./build/*
	@rmdir build
	
	@echo "Finished."

# 仅清空 /test 的中间产物
clean_test:
	@$(MAKE) -C $(TEST_DIR) clean
	@$(MAKE) -C $(VISUAL_DIR) clean