import wave
import argparse
import os
import struct
import glob

def concatenate_and_convert(wav_files, output_hex_file, output_labels_file):
    combined_data = bytearray()
    class_labels = []

    print(f"Preparing to process {len(wav_files)} file(s)...\n")

    # 1. Read and concatenate WAV data & extract labels
    for wav_file in wav_files:
        try:
            with wave.open(wav_file, 'rb') as wav:
                # Extract raw audio frames
                frames = wav.readframes(wav.getnframes())
                combined_data.extend(frames)
                
                # Extract class from filename (e.g., 'Left' -> 'left')
                filename = os.path.basename(wav_file)
                class_label = filename.split('_')[0].lower()
                class_labels.append(class_label)
                
                print(f"Processed: '{filename}' ({len(frames)} bytes) -> Class: {class_label}")
        except wave.Error as e:
            print(f"Error processing '{wav_file}': {e}")
            
    if not combined_data:
        print("\nError: No audio data extracted. Exiting.")
        return

    # 2. Pad data to ensure it's a multiple of 4 bytes (32 bits)
    remainder = len(combined_data) % 4
    if remainder != 0:
        padding = 4 - remainder
        combined_data.extend(b'\x00' * padding)
        print(f"\nNote: Added {padding} byte(s) of zero-padding to complete the final 32-bit word.")

    word_count = len(combined_data) // 4
    print(f"\nTotal concatenated size: {len(combined_data)} bytes ({word_count} 32-bit words)")
    
    # 3. Convert to 32-bit hex and write to file
    print(f"Writing 32-bit hex data to '{output_hex_file}'...")
    with open(output_hex_file, 'w') as f:
        for i in range(0, len(combined_data), 4):
            # Grab 4 bytes at a time
            chunk = combined_data[i:i+4]

            # Unpack as a Little-Endian unsigned 32-bit integer ('<I')
            word = struct.unpack('<I', chunk)[0]

            # Write the word formatted as an 8-character hex string (NO leading 0x), line separated
            f.write(f"{word:08X}\n")

    # 4. Write clip count sidecar — number of 1-second clips in this hex file.
    # The root Makefile reads this as PLAYBACK_SAMPLES_NUMBER and derives
    # XIP_N_SAMPLES = PLAYBACK_SAMPLES_NUMBER * 8000 for xip_sample_player.v.
    count_file = os.path.splitext(output_hex_file)[0] + ".count"
    num_clips = len(wav_files)
    with open(count_file, 'w') as cf:
        cf.write(f"{num_clips}\n")
    print(f"Wrote clip count ({num_clips}) to '{count_file}'")

    # 5. Write extracted class labels to text file
    print(f"Writing class labels to '{output_labels_file}'...")
    with open(output_labels_file, 'w') as lf:
        for label in class_labels:
            lf.write(f"{label}\n")

    print(f"\nSuccess! Hex data saved to '{output_hex_file}', labels saved to '{output_labels_file}', count saved to '{count_file}'")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Concatenate WAV files, extract class labels, and output to hex/text files.")
    
    parser.add_argument("directory", help="Path to the directory containing .wav files")
    parser.add_argument("-o", "--output", default="output.hex", help="Output .hex file name (default: output.hex)")
    
    # Added argument for the labels text file
    parser.add_argument("-l", "--labels", default="labels.txt", help="Output text file for class labels (default: labels.txt)")
    
    args = parser.parse_args()
    
    # Ensure the directory exists
    if not os.path.isdir(args.directory):
        print(f"Error: '{args.directory}' is not a valid directory.")
        exit(1)
        
    # Find all .wav files in the directory and sort them alphabetically
    search_pattern = os.path.join(args.directory, "*.wav")
    wav_files = sorted(glob.glob(search_pattern))
    
    # Check if we actually found any files
    if not wav_files:
        print(f"Error: No .wav files found in directory '{args.directory}'.")
        exit(1)
        
    concatenate_and_convert(wav_files, args.output, args.labels)
