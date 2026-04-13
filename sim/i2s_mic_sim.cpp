#include "i2s_mic_sim.h"
#include <fstream>
#include <iostream>

I2SMicSim::I2SMicSim(uint32_t default_val) :
    m_current_word_idx(0), m_bit_idx(0), m_last_sck(true), 
    m_ws_delayed(false), m_current_word(default_val), 
    m_sd_out(false), m_default_val(default_val) {}

void I2SMicSim::load_hex_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "I2S_MIC: Failed to open " << filename << "\n";
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines or comments
        if (line.empty() || line[0] == '#' || line[0] == '/') continue;
        uint32_t val = std::stoul(line, nullptr, 16);
        m_audio_data.push_back(val);
    }
    std::cout << "I2S_MIC: Loaded " << m_audio_data.size() << " samples.\n";
}

bool I2SMicSim::step(bool sck, bool ws) {
    // I2S data changes on the falling edge of SCK
    if (!sck && m_last_sck) {
        
        // The standard I2S format features a 1-clock delay from WS to MSB
        if (ws != m_ws_delayed) {
            
            // On a Left channel (WS=0) transition, fetch the next word
            if (ws == 0) { 
                if (m_current_word_idx < m_audio_data.size()) {
                    m_current_word = m_audio_data[m_current_word_idx++];
                } else {
                    m_current_word = m_default_val; // Fallback value
                }
            }
            
            // Reset bit index (assuming 32-bit frames)
            m_bit_idx = 31;
        } else {
            // Shift down if we are inside a frame
            if (m_bit_idx > 0) m_bit_idx--;
        }
        
        m_ws_delayed = ws;
        m_sd_out = (m_current_word >> m_bit_idx) & 1;
    }

    m_last_sck = sck;
    return m_sd_out;
}
