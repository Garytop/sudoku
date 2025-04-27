# benchmark.py
import subprocess
import statistics

TEST_CASES = [
    ("简单", 
     "530070000600195000098000060800060003400803001700020006060000280000419005000080079"),
    
    ("中等",
     "000000000000003085000020000000507000004000100090000000500000703000100000000040009"),
    
    ("困难",
     "000102000060000070008000900400000003050007000200080001009000805070000060000304000")
]

def run_solver(executable, puzzle):
    try:
        result = subprocess.run(
            [executable, puzzle],
            check=True,
            capture_output=True,
            text=True,
            timeout=30
        )
        parts = result.stdout.strip().split()
        if len(parts) == 2 and parts[0] != "No":
            return float(parts[1])
        return None
    except Exception as e:
        print(f"执行错误: {str(e)}")
        return None

def main():
    print("\n基准测试结果:")
    for name, puzzle in TEST_CASES:
        trad_times = []
        thread_times = []
        
        # 运行5次取平均
        for _ in range(5):
            if t := run_solver("./traditional_solver", puzzle):
                trad_times.append(t)
            if t := run_solver("./threaded_solver", puzzle):
                thread_times.append(t)
        
        if not trad_times or not thread_times:
            print(f"{name}测试失败")
            continue
            
        trad_avg = statistics.mean(trad_times)
        thread_avg = statistics.mean(thread_times)
        
        print(f"\n{name}数独:")
        print(f"  传统解法平均耗时: {trad_avg:.4f}s")
        print(f"  多线程解法平均耗时: {thread_avg:.4f}s")
        print(f"  性能提升: {(trad_avg - thread_avg)/trad_avg*100:.1f}%")

if __name__ == "__main__":
    main()
