# Makefile
CC := g++
CXXFLAGS := -std=c++17 -O3 -pthread -Wall
EXECUTABLES := traditional_solver threaded_solver
TEST_SCRIPT := benchmark.py

.PHONY: all build test clean

all: build test

build: $(EXECUTABLES)

traditional_solver: traditional_solver.cpp
	$(CC) $(CXXFLAGS) $< -o $@

threaded_solver: threaded_solver.cpp
	$(CC) $(CXXFLAGS) $< -o $@

test: build
	@echo "启动性能测试..."
	@python3 $(TEST_SCRIPT)
	@echo "测试完成"

clean:
	rm -f $(EXECUTABLES)
	@echo "已清理生成的可执行文件"

help:
	@echo "可用命令:"
	@echo "  make       - 构建程序并运行测试"
	@echo "  make build - 仅构建可执行文件"
	@echo "  make test  - 运行性能测试（自动构建）"
	@echo "  make clean - 清理生成的文件"
