import subprocess
import sys
import os

def run_cmd(cmd, cwd=None):
    print(f"Running: {cmd}")
    res = subprocess.run(cmd, shell=True, cwd=cwd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    if res.returncode != 0:
        print(f"Error running {cmd}")
        print(res.stderr.decode('utf-8'))
        sys.exit(1)
    return res.stdout.decode('utf-8')

def main():
    # Compile main
    print("Compiling main implementation...")
    run_cmd("wsl g++ -std=c++14 -DNDEBUG -Wall -o main.out main26b2.cpp Racenion26b2.cpp")
    
    # Compile reference
    print("Compiling reference implementation...")
    run_cmd("wsl cp ../main26b2.cpp .", cwd="ref_impl")
    run_cmd("wsl g++ -std=c++14 -DNDEBUG -Wall -o ref.out main26b2.cpp Racenion26b2.cpp", cwd="ref_impl")
    
    # Generate tests
    num_ops = 50000
    print(f"Generating {num_ops} random operations...")
    run_cmd(f"wsl python3 fuzz_generator.py {num_ops} > fuzz.in")
    
    # Run main
    print("Running main implementation...")
    run_cmd("wsl ./main.out < fuzz.in > fuzz.res")
    
    # Run reference
    print("Running reference implementation...")
    run_cmd("wsl ./ref.out < ../fuzz.in > fuzz_ref.res", cwd="ref_impl")
    
    # Diff
    print("Comparing results...")
    diff = run_cmd("wsl diff -w fuzz.res ref_impl/fuzz_ref.res")
    if diff.strip() == "":
        print("SUCCESS! No differences found.")
    else:
        print("FAILURE! Differences found.")
        # Print first 20 lines of diff
        lines = diff.split('\n')
        for line in lines[:20]:
            print(line)

if __name__ == '__main__':
    main()
