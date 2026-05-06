#!/usr/bin/env python3
import argparse
import sys
from pathlib import Path

def main():
    # Set up argument parsing
    parser = argparse.ArgumentParser(description="Create symlinks for a flat list of source files.")
    parser.add_argument('-l', '--list', required=True, help="Text file containing the flat list of source file paths.")
    parser.add_argument('-d', '--dest', required=True, help="Destination directory for the symlinks.")
    
    args = parser.parse_args()

    list_file = Path(args.list)
    dest_dir = Path(args.dest)

    # 1. Verify the list file exists
    if not list_file.exists():
        print(f"Error: The list file '{list_file}' does not exist.")
        sys.exit(1)

    # 2. Create destination directory if it doesn't exist
    dest_dir.mkdir(parents=True, exist_ok=True)

    # 3. Read paths from the list file
    with open(list_file, 'r') as f:
        # .read().split() automatically handles spaces, tabs, and newlines
        file_paths = f.read().split()

    if not file_paths:
        print("Warning: The provided list file is empty.")
        sys.exit(0)

    # 4. Create symlinks
    for path_str in file_paths:
        src_path = Path(path_str).resolve() # Use absolute path to prevent broken symlinks
        
        if not src_path.exists():
            print(f"Warning: Source file not found, skipping -> {src_path}")
            continue
            
        symlink_target = dest_dir / src_path.name

        # Handle existing files/symlinks gracefully
        if symlink_target.exists() or symlink_target.is_symlink():
            print(f"Skipped (already exists): {symlink_target.name}")
        else:
            try:
                symlink_target.symlink_to(src_path)
                print(f"Linked: {symlink_target.name} -> {src_path}")
            except OSError as e:
                print(f"Error linking {src_path.name}: {e}")

if __name__ == "__main__":
    main()
