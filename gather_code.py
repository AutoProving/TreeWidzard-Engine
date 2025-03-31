#!/usr/bin/env python3
"""
Gather code from multiple C++ source files (.h, .cpp)
and assemble a prompt for use with an LLM, with support
for excluding specific subdirectories.
"""

import argparse
from pathlib import Path

def gather_code(file_paths):
    """
    Read the contents of multiple files and return
    a combined string, formatted for C++ code.
    """
    combined_code = []
    for file_path in file_paths:
        with open(file_path, 'r', encoding='utf-8') as f:
            file_contents = f.read()
        
        combined_code.append(f"### File: {file_path}\n```cpp\n{file_contents}\n```\n\n")
    
    return "".join(combined_code)

def get_file_paths_in_directory(directory, extensions=None, recursive=False, exclude_dirs=None):
    """
    Returns a list of file paths with the given extensions in a directory.
    Supports excluding specific subdirectories.
    """
    if extensions is None:
        extensions = [".cpp", ".h"]
    if exclude_dirs is None:
        exclude_dirs = []

    directory = Path(directory)
    file_paths = []

    for ext in extensions:
        if recursive:
            candidates = directory.rglob(f"*{ext}")
        else:
            candidates = directory.glob(f"*{ext}")
        
        for file_path in candidates:
            # Check if file is in an excluded directory
            if any(excluded in file_path.parts for excluded in exclude_dirs):
                continue
            file_paths.append(file_path)
    
    return file_paths

def main():
    parser = argparse.ArgumentParser(
        description="Gather code from multiple C++ source files to form a single prompt, with support for excluding directories."
    )
    parser.add_argument(
        "--dir", 
        type=str, 
        required=True,
        help="Path to the directory containing C++ code files."
    )
    parser.add_argument(
        "--recursive",
        action="store_true",
        help="Recursively scan subdirectories (default: False)."
    )
    parser.add_argument(
        "--exclude",
        type=str,
        nargs="*",
        default=[],
        help="List of subdirectory names to exclude from the search."
    )

    args = parser.parse_args()

    # Collect all .h and .cpp files, excluding specific directories
    file_paths = get_file_paths_in_directory(
        args.dir,
        extensions=[".cpp", ".h"],
        recursive=args.recursive,
        exclude_dirs=args.exclude
    )

    # Combine contents into a single string
    prompt_text = gather_code(file_paths)

    # Print out the prompt to the console
    print("======== BEGIN LLM PROMPT ========")
    print(prompt_text)
    print("======== END LLM PROMPT ========")

if __name__ == "__main__":
    main()
