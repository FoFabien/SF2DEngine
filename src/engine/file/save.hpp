#ifndef SAVE_HPP
#define SAVE_HPP
#define _USE_SAVE_
#ifdef _USE_SAVE_

#include <string>
#include <vector>
#include <stdint.h>

struct SlotState
{
    bool used;
    std::string header;
};

class Save
{
    public:
        Save();
        ~Save();
        /// initialize the save system - only have to be called at the start
        bool init(const std::string& dir, const std::string& version, const int32_t maxSlot);
        /// load & save the master file
        bool loadSystem();
        bool saveSystem();
        /// load & save a specific save file
        bool loadSlot(); // current
        bool loadSlot(int32_t s);
        bool saveSlot(); // current
        bool saveSlot(int32_t s);
        /// change the current save slot
        void setSlot(int32_t s);
        /// refresh the slot states
        void refreshSlotState();
        /// return the slot states
        std::vector<SlotState> getSlotState() const;
        /// set the current slot header
        void setCurrentSlotHeader(const std::string& heade);
    protected:
        bool ready;
        std::string save_dir;
        std::string version_string;
        int32_t max_slot;
        int32_t current_slot;
        std::vector<SlotState> states;
};

#endif
#endif // SAVE_HPP
