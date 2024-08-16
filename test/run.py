import os
import argparse
import subprocess

def run():
    parser = argparse.ArgumentParser()
    parser.add_argument("-t",
                        "--test-suite",
                        type=str,
                        choices=["parser"],
                        help="the test suite to run. If not provided, all tests are run.")
    args = parser.parse_args()

    sif_build = "./build/sif"

    if args.test_suite:
        print(f"Running tests in suite: '{args.test_suite}'")
        # TODO: support individual suites
    else:
        print("Running all available test suites")
        test_dir = "./test"
        folders = [f for f in os.listdir(test_dir) if os.path.isdir(os.path.join(test_dir, f))]
        for folder in folders:
            folder_path = os.path.join(test_dir, folder)
            for input in os.listdir(folder_path):
                input_path = os.path.join(folder_path, input)
                result = subprocess.run([sif_build, input_path])
                assert result.returncode == 0, f"test '{input}' failed"

if __name__ == "__main__":
    run()
