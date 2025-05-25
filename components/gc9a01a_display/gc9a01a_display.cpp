#include "gc9a01a_display.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/color.h"

namespace esphome
{
    namespace gc9a01a_display
    {

        static const char *const TAG = "gc9a01a_display";

        void GC9A01ADisplay::setup()
        {
            ESP_LOGCONFIG(TAG, "Setting up GC9A01A display...");

            this->spi_setup();

            this->dc_pin_->setup();
            this->dc_pin_->digital_write(false);

            if (this->reset_pin_ != nullptr)
            {
                this->reset_pin_->setup();
                this->reset_pin_->digital_write(true);
            }

            if (this->backlight_pin_ != nullptr)
            {
                this->backlight_pin_->setup();
                this->backlight_pin_->digital_write(true);
            }

            this->is_ready_ = true;

            // // Initialize pins
            // this->spi_setup();

            // this->dc_pin_->setup();
            // this->dc_pin_->digital_write(false);

            // if (this->reset_pin_ != nullptr)
            // {
            //     this->reset_pin_->setup();
            //     this->reset_pin_->digital_write(true);
            // }

            // if (this->backlight_pin_ != nullptr)
            // {
            //     this->backlight_pin_->setup();
            //     this->backlight_pin_->digital_write(false);
            // }

            // // Hardware reset
            // if (this->reset_pin_ != nullptr)
            // {
            //     this->reset_pin_->digital_write(false);
            //     delay(10);
            //     this->reset_pin_->digital_write(true);
            //     delay(120);
            // }

            // this->init_display_();

            // // Turn on backlight
            // if (this->backlight_pin_ != nullptr)
            // {
            //     this->backlight_pin_->digital_write(true);
            // }

            // this->is_ready_ = true;
            // ESP_LOGI(TAG, "Display initialization complete - ready: %s", this->is_ready_ ? "true" : "false");

            // // Intial fill with red
            // ESP_LOGI(TAG, "Testing display with red fill...");
            // Color red_color = Color(255, 0, 0);
            // this->fill(red_color);
        }

        void GC9A01ADisplay::update()
        {
            // Lifecycle logging - commented out to reduce spam
            // ESP_LOGD(TAG, "Update called - is_ready: %s", this->is_ready_ ? "true" : "false");

            if (!this->is_ready_)
            {
                ESP_LOGW(TAG, "Display not ready, skipping update");
                return;
            }

            // One-time minimal test
            static bool minimal_test_done = false;
            static uint32_t test_delay_counter = 0;

            if (!minimal_test_done)
            {
                test_delay_counter++;

                if (test_delay_counter == 10)
                { // Wait 10 seconds
                    ESP_LOGI(TAG, "=== MINIMAL PIN CONTROL TEST ===");

                    // Test DC pin control
                    ESP_LOGI(TAG, "DC pin current state: %s", this->dc_pin_->digital_read() ? "HIGH" : "LOW");

                    this->dc_pin_->digital_write(true);
                    ESP_LOGI(TAG, "After setting DC HIGH: %s", this->dc_pin_->digital_read() ? "HIGH" : "LOW");

                    this->dc_pin_->digital_write(false);
                    ESP_LOGI(TAG, "After setting DC LOW: %s", this->dc_pin_->digital_read() ? "HIGH" : "LOW");

                    // Test Reset pin control
                    if (this->reset_pin_ != nullptr)
                    {
                        ESP_LOGI(TAG, "Reset pin current state: %s", this->reset_pin_->digital_read() ? "HIGH" : "LOW");
                        this->reset_pin_->digital_write(false);
                        ESP_LOGI(TAG, "After setting Reset LOW: %s", this->reset_pin_->digital_read() ? "HIGH" : "LOW");
                        this->reset_pin_->digital_write(true);
                        ESP_LOGI(TAG, "After setting Reset HIGH: %s", this->reset_pin_->digital_read() ? "HIGH" : "LOW");
                    }

                    ESP_LOGI(TAG, "=== MINIMAL TEST COMPLETE ===");
                    minimal_test_done = true;
                }
                else
                {
                    ESP_LOGI(TAG, "Minimal test countdown: %d seconds", 10 - test_delay_counter);
                }
            }

            // ESP_LOGD(TAG, "Starting do_update_()");
            this->do_update_();
            // ESP_LOGD(TAG, "Update cycle complete");

            // Diagnostic logging every 20 updates
            this->update_counter_++;

            if (this->update_counter_ % 20 == 0)
            {
                ESP_LOGI(TAG, "=== Diagnostic Report (Update #%d) ===", this->update_counter_);
                ESP_LOGI(TAG, "Component Status:");
                ESP_LOGI(TAG, "  Ready State: %s", this->is_ready_ ? "true" : "false");
                ESP_LOGI(TAG, "  Display Dimensions: %dx%d", this->get_width_internal(), this->get_height_internal());
                ESP_LOGI(TAG, "  Display Type: %d", (int)this->get_display_type());
                ESP_LOGI(TAG, "=== End Diagnostic Report ===");
            }
        }

        void GC9A01ADisplay::dump_config()
        {
            ESP_LOGCONFIG(TAG, "GC9A01A Display:");
            LOG_PIN("  DC Pin: ", this->dc_pin_);
            LOG_PIN("  Reset Pin: ", this->reset_pin_);
            LOG_PIN("  Backlight Pin: ", this->backlight_pin_);
            ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", this->get_width_internal(), this->get_height_internal());
        }

        float GC9A01ADisplay::get_setup_priority() const
        {
            return setup_priority::HARDWARE; // Match ILI9XXX pattern
        }

        void GC9A01ADisplay::fill(Color color)
        {
            ESP_LOGI(TAG, "Fill called with color R:%d G:%d B:%d, ready: %s",
                     color.red, color.green, color.blue, this->is_ready_ ? "true" : "false");

            if (!this->is_ready_)
                return;

            uint16_t color565 = this->color_to_565_(color);
            ESP_LOGI(TAG, "Converted to RGB565: 0x%04X", color565);

            // Debug: Log what we're about to do
            ESP_LOGI(TAG, "Setting address window to full screen: 0,0 to %d,%d",
                     GC9A01A_WIDTH - 1, GC9A01A_HEIGHT - 1);

            this->set_addr_window_(0, 0, GC9A01A_WIDTH - 1, GC9A01A_HEIGHT - 1);

            ESP_LOGI(TAG, "Writing %d pixels of color 0x%04X",
                     GC9A01A_WIDTH * GC9A01A_HEIGHT, color565);

            this->write_color_(color565, GC9A01A_WIDTH * GC9A01A_HEIGHT);

            ESP_LOGI(TAG, "Fill operation complete");
        }

        void GC9A01ADisplay::draw_absolute_pixel_internal(int x, int y, Color color)
        {
            if (x < 0 || x >= this->get_width_internal() || y < 0 || y >= this->get_height_internal())
            {
                return;
            }

            if (!this->is_ready_)
            {
                ESP_LOGW(TAG, "Display not ready for pixel draw");
                return;
            }

            uint16_t color565 = this->color_to_565_(color);
            this->set_addr_window_(x, y, x, y);
            this->write_data_16_(color565);
        }

        int GC9A01ADisplay::get_height_internal() { return GC9A01A_HEIGHT; }

        int GC9A01ADisplay::get_width_internal() { return GC9A01A_WIDTH; }

        display::DisplayType GC9A01ADisplay::get_display_type() { return display::DisplayType::DISPLAY_TYPE_COLOR; }

        void GC9A01ADisplay::init_display_()
        {
            ESP_LOGD(TAG, "Initializing GC9A01A display...");

            // Software reset
            this->write_command_(GC9A01A_SWRESET);
            delay(120);

            // Sleep out
            this->write_command_(GC9A01A_SLPOUT);
            delay(120);

            // Pixel format: 16 bits per pixel (RGB565)
            this->write_command_(GC9A01A_COLMOD);
            this->write_data_(0x55);

            // Memory access control (rotation and color order)
            this->write_command_(GC9A01A_MADCTL);
            uint8_t madctl = GC9A01A_MADCTL_BGR; // Set BGR color order
            this->write_data_(madctl);

            // GC9A01A specific initialization sequence
            this->write_command_(0xEF);

            this->write_command_(0xEB);
            this->write_data_(0x14);

            this->write_command_(0xFE);
            this->write_command_(0xEF);

            this->write_command_(0xEB);
            this->write_data_(0x14);

            this->write_command_(0x84);
            this->write_data_(0x40);

            this->write_command_(0x85);
            this->write_data_(0xFF);

            this->write_command_(0x86);
            this->write_data_(0xFF);

            this->write_command_(0x87);
            this->write_data_(0xFF);

            this->write_command_(0x88);
            this->write_data_(0x0A);

            this->write_command_(0x89);
            this->write_data_(0x21);

            this->write_command_(0x8A);
            this->write_data_(0x00);

            this->write_command_(0x8B);
            this->write_data_(0x80);

            this->write_command_(0x8C);
            this->write_data_(0x01);

            this->write_command_(0x8D);
            this->write_data_(0x01);

            this->write_command_(0x8E);
            this->write_data_(0xFF);

            this->write_command_(0x8F);
            this->write_data_(0xFF);

            this->write_command_(0xB6);
            this->write_data_(0x00);
            this->write_data_(0x20);

            this->write_command_(0x36);
            this->write_data_(madctl);

            this->write_command_(0x3A);
            this->write_data_(0x05);

            this->write_command_(0x90);
            this->write_data_(0x08);
            this->write_data_(0x08);
            this->write_data_(0x08);
            this->write_data_(0x08);

            this->write_command_(0xBD);
            this->write_data_(0x06);

            this->write_command_(0xBC);
            this->write_data_(0x00);

            this->write_command_(0xFF);
            this->write_data_(0x60);
            this->write_data_(0x01);
            this->write_data_(0x04);

            this->write_command_(0xC3);
            this->write_data_(0x13);

            this->write_command_(0xC4);
            this->write_data_(0x13);

            this->write_command_(0xC9);
            this->write_data_(0x22);

            this->write_command_(0xBE);
            this->write_data_(0x11);

            this->write_command_(0xE1);
            this->write_data_(0x10);
            this->write_data_(0x0E);

            this->write_command_(0xDF);
            this->write_data_(0x21);
            this->write_data_(0x0C);
            this->write_data_(0x02);

            this->write_command_(0xF0);
            this->write_data_(0x45);
            this->write_data_(0x09);
            this->write_data_(0x08);
            this->write_data_(0x08);
            this->write_data_(0x26);
            this->write_data_(0x2A);

            this->write_command_(0xF1);
            this->write_data_(0x43);
            this->write_data_(0x70);
            this->write_data_(0x72);
            this->write_data_(0x36);
            this->write_data_(0x37);
            this->write_data_(0x6F);

            this->write_command_(0xF2);
            this->write_data_(0x45);
            this->write_data_(0x09);
            this->write_data_(0x08);
            this->write_data_(0x08);
            this->write_data_(0x26);
            this->write_data_(0x2A);

            this->write_command_(0xF3);
            this->write_data_(0x43);
            this->write_data_(0x70);
            this->write_data_(0x72);
            this->write_data_(0x36);
            this->write_data_(0x37);
            this->write_data_(0x6F);

            this->write_command_(0xED);
            this->write_data_(0x1B);
            this->write_data_(0x0B);

            this->write_command_(0xAE);
            this->write_data_(0x77);

            this->write_command_(0xCD);
            this->write_data_(0x63);

            this->write_command_(0x70);
            this->write_data_(0x07);
            this->write_data_(0x07);
            this->write_data_(0x04);
            this->write_data_(0x0E);
            this->write_data_(0x0F);
            this->write_data_(0x09);
            this->write_data_(0x07);
            this->write_data_(0x08);
            this->write_data_(0x03);

            this->write_command_(0xE8);
            this->write_data_(0x34);

            this->write_command_(0x62);
            this->write_data_(0x18);
            this->write_data_(0x0D);
            this->write_data_(0x71);
            this->write_data_(0xED);
            this->write_data_(0x70);
            this->write_data_(0x70);
            this->write_data_(0x18);
            this->write_data_(0x0F);
            this->write_data_(0x71);
            this->write_data_(0xEF);
            this->write_data_(0x70);
            this->write_data_(0x70);

            this->write_command_(0x63);
            this->write_data_(0x18);
            this->write_data_(0x11);
            this->write_data_(0x71);
            this->write_data_(0xF1);
            this->write_data_(0x70);
            this->write_data_(0x70);
            this->write_data_(0x18);
            this->write_data_(0x13);
            this->write_data_(0x71);
            this->write_data_(0xF3);
            this->write_data_(0x70);
            this->write_data_(0x70);

            this->write_command_(0x64);
            this->write_data_(0x28);
            this->write_data_(0x29);
            this->write_data_(0xF1);
            this->write_data_(0x01);
            this->write_data_(0xF1);
            this->write_data_(0x00);
            this->write_data_(0x07);

            this->write_command_(0x66);
            this->write_data_(0x3C);
            this->write_data_(0x00);
            this->write_data_(0xCD);
            this->write_data_(0x67);
            this->write_data_(0x45);
            this->write_data_(0x45);
            this->write_data_(0x10);
            this->write_data_(0x00);
            this->write_data_(0x00);
            this->write_data_(0x00);

            this->write_command_(0x67);
            this->write_data_(0x00);
            this->write_data_(0x3C);
            this->write_data_(0x00);
            this->write_data_(0x00);
            this->write_data_(0x00);
            this->write_data_(0x01);
            this->write_data_(0x54);
            this->write_data_(0x10);
            this->write_data_(0x32);
            this->write_data_(0x98);

            this->write_command_(0x74);
            this->write_data_(0x10);
            this->write_data_(0x85);
            this->write_data_(0x80);
            this->write_data_(0x00);
            this->write_data_(0x00);
            this->write_data_(0x4E);
            this->write_data_(0x00);

            this->write_command_(0x98);
            this->write_data_(0x3E);
            this->write_data_(0x07);

            this->write_command_(GC9A01A_INVON); // Display inversion on
            delay(10);

            this->write_command_(GC9A01A_NORON); // Normal display mode on
            delay(10);

            this->write_command_(GC9A01A_DISPON); // Display on
            delay(120);

            ESP_LOGD(TAG, "GC9A01A display initialization complete");
        }

        void GC9A01ADisplay::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
        {
            this->write_command_(GC9A01A_CASET); // Column address set
            this->write_data_16_(x1);
            this->write_data_16_(x2);

            this->write_command_(GC9A01A_RASET); // Row address set
            this->write_data_16_(y1);
            this->write_data_16_(y2);

            this->write_command_(GC9A01A_RAMWR); // Write to RAM
        }

        void GC9A01ADisplay::write_command_(uint8_t cmd)
        {
            ESP_LOGV(TAG, "Writing command: 0x%02X", cmd);
            this->enable_();
            this->dc_pin_->digital_write(false);
            this->write_byte(cmd);
            this->disable_();
        }

        void GC9A01ADisplay::write_data_(uint8_t data)
        {
            ESP_LOGV(TAG, "Writing data: 0x%02X", data);
            this->enable_();
            this->dc_pin_->digital_write(true);
            this->write_byte(data);
            this->disable_();
        }

        void GC9A01ADisplay::write_data_16_(uint16_t data)
        {
            this->enable_();
            this->dc_pin_->digital_write(true);
            this->write_byte(data >> 8);
            this->write_byte(data & 0xFF);
            this->disable_();
        }

        void GC9A01ADisplay::write_color_(uint16_t color, uint32_t count)
        {
            this->enable_();
            this->dc_pin_->digital_write(true);

            uint8_t color_high = color >> 8;
            uint8_t color_low = color & 0xFF;

            for (uint32_t i = 0; i < count; i++)
            {
                this->write_byte(color_high);
                this->write_byte(color_low);
            }

            this->disable_();
        }

        uint16_t GC9A01ADisplay::color_to_565_(Color color)
        {
            uint16_t r = (color.red & 0xF8) << 8;
            uint16_t g = (color.green & 0xFC) << 3;
            uint16_t b = color.blue >> 3;
            return r | g | b;
        }

        void GC9A01ADisplay::enable_()
        {
            // CS pin control is handled by SPI device base class
        }

        void GC9A01ADisplay::disable_()
        {
            // CS pin control is handled by SPI device base class
        }

    } // namespace gc9a01a_display
} // namespace esphome