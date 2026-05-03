import wave
import argparse
import os
import struct

def concatenate_and_convert(wav_files, output_hex_file):
    combined_data = bytearray()

    print(f"Preparing to process {len(wav_files)} file(s)...\n")

    # 1. Read and concatenate WAV data
    for wav_file in wav_files:
        if not os.path.exists(wav_file):
            print(f"Warning: '{wav_file}' not found. Skipping.")
            continue
            
        try:
            with wave.open(wav_file, 'rb') as wav:
                # Extract raw audio frames
                frames = wav.readframes(wav.getnframes())
                combined_data.extend(frames)
                print(f"Processed: '{wav_file}' ({len(frames)} bytes)")
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
    print(f"Writing 32-bit hex data to '{output_hex_file}'...")
    
    # 3. Convert to 32-bit hex and write to file
    with open(output_hex_file, 'w') as f:
        for i in range(0, len(combined_data), 4):
            # Grab 4 bytes at a time
            chunk = combined_data[i:i+4]
            
            # Unpack as a Little-Endian unsigned 32-bit integer ('<I')
            # If your target hardware requires Big-Endian, change '<I' to '>I'
            word = struct.unpack('<I', chunk)[0]
            
            # Write the word formatted as an 8-character hex string (NO leading 0x), line separated
            f.write(f"{word:08X}\n")
                
    print(f"\nSuccess! Data saved to {output_hex_file}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Concatenate WAV files and convert raw audio to line-separated 32-bit hex words.")
    
    parser.add_argument("input_files", nargs='+', help="List of input .wav files (e.g., file1.wav file2.wav)")
    parser.add_argument("-o", "--output", default="output.hex", help="Output .hex file name (default: output.hex)")
    
    args = parser.parse_args()
    
    concatenate_and_convert(args.input_files, args.output)
