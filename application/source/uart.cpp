/// @file uart.cpp
/// @author Denver Hoggatt
/// @brief UART definitions
///
/// Copyright (c) 2025 Denver Hoggatt. All rights reserved.
///
/// This software is licensed under terms that can be found in the LICENSE file
/// in the root directory of this software component.
/// If no LICENSE file comes with this software, it is provided AS-IS.
///

#include "uart.hpp"
#include "uart_hal.hpp"
#include "event.hpp"
#include "macros.hpp"

#include <cstring>
#include <atomic>
#include <cstdio>
#include <cinttypes>

//--------------------------------------------------------------------------------------------------
//  Macros and Error Checking
//--------------------------------------------------------------------------------------------------

namespace
{
    //----------------------------------------------------------------------------------------------
    //  Private Constants
    //----------------------------------------------------------------------------------------------

    constexpr uint32_t RCVD_QUEUE_SIZE = 64;

    //----------------------------------------------------------------------------------------------
    //  Private Data Types
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Private Function Prototypes
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  File Variables
    //----------------------------------------------------------------------------------------------

    bool                      isr_enabled      = false;
    bool                      processing_read  = false;
    uint16_t                  rcvd_queue_front = 0;
    std::atomic_uint_fast16_t rcvd_queue_rear;
    char                      rcvd_queue[RCVD_QUEUE_SIZE];

    //----------------------------------------------------------------------------------------------
    //  Private Functions
    //----------------------------------------------------------------------------------------------


    // End of Anonymous Namespace
}

namespace uart
{
    //----------------------------------------------------------------------------------------------
    //  Public Functions
    //----------------------------------------------------------------------------------------------

    /// @brief UART handling ISR.
    /// @param c Character received from the UART.
    ///
    void isr_read(char c)
    {
        volatile uint16_t pos      = rcvd_queue_rear.fetch_add(1) % RCVD_QUEUE_SIZE;
        volatile uint16_t next_pos = (pos + 1) % RCVD_QUEUE_SIZE;

        INVAR(next_pos != (rcvd_queue_front % RCVD_QUEUE_SIZE), error::QueueOverflow);

        rcvd_queue[pos] = c;

        if (!isr_enabled)
        {
            return;
        }

        event::post(event::ID::control_UARTInput, (void *)0);
    }

    //----------------------------------------------------------------------------------------------
    //  Class Function Definitions
    //----------------------------------------------------------------------------------------------

    /// @brief Prints
    /// @param data
    /// @param dir
    void UART::print(void *data, io::IODirection dir)
    {
        io::print("UART", this->name, this->id, (char *)data, dir);
    }

    /// @brief Gets the input data.
    /// @return Input data.
    ///
    void *UART::get_by_id()
    {
        static char ret_buf[RCVD_QUEUE_SIZE + 1];

        uint16_t rear = rcvd_queue_rear.load() % RCVD_QUEUE_SIZE;

        std::atomic<uint32_t> memory_barrier; // Prevent re-ordering
        memory_barrier.load();

        memset(ret_buf, '\0', RCVD_QUEUE_SIZE + 1);

        uint32_t cnt = 0;
        while (rcvd_queue_front != rear)
        {
            INVAR(cnt <= RCVD_QUEUE_SIZE, error::InvalidLength);

            ret_buf[cnt]     = rcvd_queue[rcvd_queue_front];
            rcvd_queue_front = (rcvd_queue_front + 1) % RCVD_QUEUE_SIZE;
            cnt++;
        }

        return ret_buf;
    }

    /// @brief Sets the output data.
    /// @param data Data to set.
    ///
    void UART::set_output(void *data)
    {
        const char *write_str = (const char *)data;

        uart_hal::send(this->uart_port, write_str);
    }

    /// @brief Initializes the IO.
    ///
    void UART::init()
    {
        REENTRY_GUARD_CLASS();

        REQUIRE(std::atomic_is_lock_free(&rcvd_queue_rear), error::DeviceInitFailed);

        rcvd_queue_rear = 0;

        this->init_input_info(&typeid(const char *), io::IOType::UART);
        this->init_output_info(&typeid(const char *), io::IOType::UART);

        error::Error err = uart_hal::open(this->uart_port);

        isr_enabled = true;

        ENSURE(err == error::NoError, error::DeviceInitFailed);
    }

    //----------------------------------------------------------------------------------------------
    //  Class Operator Definitions
    //----------------------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------------------
    //  Class Constructor Definitions
    //----------------------------------------------------------------------------------------------


    // End of Namespace
}

//--------------------------------------------------------------------------------------------------
// Global Namespace Functions
//--------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------------------
// Unit Test Accessors
//--------------------------------------------------------------------------------------------------

namespace uart_test
{


}

// End of File