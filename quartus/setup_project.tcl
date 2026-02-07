# quartus/setup_proj.tcl

# 1. Load the Quartus Project package
package require ::quartus::project

# 2. Get variables from the Shell Environment (passed by Makefile)
set project_name  $::env(QUARTUS_PROJECT)
set family        $::env(QUARTUS_FAMILY)
set device        $::env(QUARTUS_DEVICE)
set top_module    $::env(QUARTUS_TOP_MODULE)
set src_files     $::env(QUARTUS_SRCS)
set constraint_fn $::env(QUARTUS_CONSTRAINTS)

# 3. Create (or overwrite) the project
# Note: We use -overwrite to ensure a clean state every time you run 'make config'
if {[project_exists $project_name]} {
    project_open -current_revision $project_name
} else {
    project_new -overwrite -family $family -part $device $project_name
}

# 4. Apply Global Settings
set_global_assignment -name FAMILY $family
set_global_assignment -name DEVICE $device
set_global_assignment -name TOP_LEVEL_ENTITY $top_module
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files

# 5. Add Source Files
# We treat the environment variable as a list.
foreach file $src_files {
    # Filter out empty strings (caused by extra spaces in Makefile)
    if {$file == ""} { continue }

    # OPTIONAL: Filter out .vh files if they are just headers
    # Quartus prefers headers to be found via search path, not added as sources
    if {[string match "*.vh" $file]} {
        post_message -type info "Skipping header file: $file"
        continue 
    }

    # Add the file (assuming Verilog/SystemVerilog)
    post_message -type info "Adding source: $file"
    set_global_assignment -name VERILOG_FILE $file
}

# 6. Source the Constraints Tcl file (if it exists)
if {[file exists $constraint_fn]} {
    post_message -type info "Sourcing constraints: $constraint_fn"
    source $constraint_fn
} else {
    post_message -type warning "Constraints file not found: $constraint_fn"
}

# 7. Commit changes and close
export_assignments
project_close
