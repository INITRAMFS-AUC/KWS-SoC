#ifndef I2S_MIC_SIM_H
#define I2S_MIC_SIM_H

#include <string>
#include <vector>
#include <cstdint>

class I2SMicSim {
private:
    std::vector<uint32_t> m_audio_data;
    size_t m_current_word_idx;
    int m_bit_idx;
    bool m_last_sck;
    bool m_ws_delayed;
    uint32_t m_current_word;
    bool m_sd_out;
    uint32_t m_default_val;

public:
    // Initializes the mic with a default fallback value once the buffer runs out
    I2SMicSim(uint32_t default_val = 0);
    
    // Loads hexadecimal audio samples line by line
    void load_hex_file(const std::string& filename);
    
    // Simulates one tick. Call this every TB cycle.
    bool step(bool sck, bool ws);
};

#endif
