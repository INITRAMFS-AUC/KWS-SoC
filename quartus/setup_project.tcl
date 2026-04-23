package require ::quartus::project

# Get variables from Makefile
set project_name  $::env(QUARTUS_PROJECT)
set family        $::env(QUARTUS_FAMILY)
set device        $::env(QUARTUS_DEVICE)
set top_module    $::env(QUARTUS_TOP_MODULE)
set src_files     $::env(QUARTUS_SRCS)
set constraint_fn $::env(QUARTUS_CONSTRAINTS)
set nproc 	  $::env(NPROC)

if {[project_exists $project_name]} {
    project_open -current_revision $project_name
} else {
    project_new -overwrite -family $family -part $device $project_name
}

set_global_assignment -name FAMILY $family
set_global_assignment -name DEVICE $device
set_global_assignment -name TOP_LEVEL_ENTITY $top_module
set_global_assignment -name PROJECT_OUTPUT_DIRECTORY output_files
set_global_assignment -name NUM_PARALLEL_PROCESSORS $nproc

# --- ADD FILES ---
foreach file $src_files {
    if {$file == ""} { continue }

    # 1. Ensure path is normalized (handles symlinks/.. cleanly)
    set abs_file [file normalize $file]
    set ext [file extension $abs_file]

    # 2. Assign Type based on extension
    switch -- $ext {
        ".qip" {
            post_message -type info "Adding IP Manifest: $abs_file"
            set_global_assignment -name QIP_FILE $abs_file
        }
        ".v" - ".sv" {
            post_message -type info "Adding HDL Source: $abs_file"
            set_global_assignment -name VERILOG_FILE $abs_file
        }
        ".vh" - ".svh" {
            # Add header to project for visibility
            post_message -type info "Adding Header: $abs_file"
            set_global_assignment -name VERILOG_INCLUDE_FILE $abs_file
	    set_global_assignment -name SEARCH_PATH [file dirname $abs_file]
            
	    continue
        }
        default {
            post_message -type warning "Unknown file type: $abs_file"
            set_global_assignment -name VERILOG_FILE $abs_file
        }
    }
}

# Source Constraints
if {[file exists $constraint_fn]} {
    post_message -type info "Sourcing constraints: $constraint_fn"
    source $constraint_fn
} else {
    post_message -type warning "Constraints file NOT FOUND at: $constraint_fn"
}

# Register SDC timing constraints (lives alongside the QSF in quartus/)
set sdc_file [file normalize [file join [file dirname $constraint_fn] "../KWS-SoC.sdc"]]
if {[file exists $sdc_file]} {
    post_message -type info "Adding SDC timing constraints: $sdc_file"
    set_global_assignment -name SDC_FILE $sdc_file
} else {
    post_message -type warning "SDC timing constraints NOT FOUND at: $sdc_file"
}

export_assignments
project_close
