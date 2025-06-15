/// @file adc-test.hpp
/// Unit test accessor declarations for the adc module.

#ifndef ADC_TEST_H
    #define ADC_TEST_H

namespace adc_test
{
    //--------------------------------------------------------------------------
    //  Accessor Functions
    //--------------------------------------------------------------------------

    /// @brief Gets the address where the ADC DMA happens.
    /// @return ADC DMA address.
    ///
    volatile uint16_t **get_dma();

    // End of Namespace
}

#endif

// End of File