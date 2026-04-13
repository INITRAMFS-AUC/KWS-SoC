#!/usr/bin/env python3
import re
import sys

if len(sys.argv) != 3:
    print("Usage: python3 gen_inst_params.py <input.vh> <output.vh>")
    sys.exit(1)

infile = sys.argv[1]
outfile = sys.argv[2]

with open(infile, "r") as f:
    lines = f.readlines()

parsed_lines = []
last_param_idx = -1
params_dict = {}

for line in lines:
    stripped = line.strip()
    if stripped.startswith("parameter "):
        # 1. Separate any inline comment from the code
        if "//" in stripped:
            code_part, comment_part = line.split("//", 1)
            comment_part = " //" + comment_part
        else:
            code_part = line
            comment_part = "\n"

        # 2. Clean up the code part and trailing comma
        code_part = code_part.replace("parameter", "", 1).strip()
        if code_part.endswith(","):
            code_part = code_part[:-1].strip()

        # 3. Extract Name and Value
        if "=" in code_part:
            name, value = code_part.split("=", 1)
            name = name.strip()
            value = value.strip()

            # 4. Substitute previously defined parameters
            # Sort by length descending to replace longer names first, preventing partial overlaps
            for k in sorted(params_dict.keys(), key=len, reverse=True):
                # \b ensures we only match whole words
                value = re.sub(rf"\b{k}\b", params_dict[k], value)

            # Store the fully resolved value for future substitutions further down the file
            params_dict[name] = value

            # Format as an instantiation override
            new_line = f"    .{name} ({value}),{comment_part}"
            if not new_line.endswith("\n"):
                new_line += "\n"

            parsed_lines.append(new_line)
            last_param_idx = len(parsed_lines) - 1
    else:
        # Pass through empty lines and full-line comments
        if stripped == "" or stripped.startswith("//"):
            parsed_lines.append(line)

# 5. Safely strip the trailing comma from the very last parameter
if last_param_idx != -1:
    line = parsed_lines[last_param_idx]
    idx = line.rfind("),")
    if idx != -1:
        parsed_lines[last_param_idx] = line[:idx] + ")" + line[idx + 2 :]

with open(outfile, "w") as f:
    f.writelines(parsed_lines)
