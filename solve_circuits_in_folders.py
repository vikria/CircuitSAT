import subprocess
import os
from threading import Timer


def kill_process(process, path_res_file: str) -> None:
    process.kill()
    with open(path_res_file, 'a') as f:
        f.write('TIMEOUT \n')


def solve_circuits_in_folders(path_solver: str, path_folder: str, path_res_file: str, time_limit: int) -> None:

    for bench_file in os.listdir(path_folder):
        print(bench_file)

        subproc = subprocess.Popen([
            path_solver,
            os.path.join(path_folder, bench_file),
            path_res_file
        ])

        timer = Timer(time_limit, kill_process, [subproc, path_res_file])
        try:
            timer.start()
            subproc.communicate()
        finally:
            timer.cancel()


if __name__ == "__main__":
    # WARNING: need to substitute valid paths
    solve_circuits_in_folders("./CircuitSAT", "./example", "./result.txt", 10)
