#include <iostream>
#include <libusb-1.0/libusb.h>
#include <thread>
#include <chrono>
#include <cstring>
#include <string>
#include <iomanip>

void cleanup(libusb_device_handle* handle, libusb_context* ctx) {
    if (handle) {
        libusb_release_interface(handle, 0);
        libusb_close(handle);
    }
    if (ctx) {
        libusb_exit(ctx);
    }
}

std::string bytes_to_string(const unsigned char* data, int length) {
    return std::string(reinterpret_cast<const char*>(data), length);
}

int main() {
    libusb_device_handle *handle = nullptr;
    libusb_context *ctx = nullptr;
    unsigned char data[1024];
    int actual_length;

    if (libusb_init(&ctx) < 0) {
        std::cerr << "Não foi possível inicializar libusb." << std::endl;
        return 1;
    }

    handle = libusb_open_device_with_vid_pid(ctx, 0x0483, 0xdf11);
    if (handle == nullptr) {
        std::cerr << "Não foi possível abrir o dispositivo." << std::endl;
        cleanup(nullptr, ctx);
        return 1;
    }

    if (libusb_claim_interface(handle, 0) < 0) {
        std::cerr << "Não foi possível reivindicar a interface." << std::endl;
        cleanup(handle, ctx);
        return 1;
    }

    unsigned char request_type = LIBUSB_ENDPOINT_IN | LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE;
    unsigned char request = 0x00; // DFU_UPLOAD
    unsigned short value = 0x0000;
    unsigned short index = 0x0000;

    bool running = true;
    while (running) {
        std::memset(data, 0, sizeof(data));

        int result = libusb_control_transfer(handle, request_type, request, value, index, data, sizeof(data), 1000);

        if (result >= 0) {
            std::cout << "Dados lidos (" << result << " bytes): ";

            // Convertendo os bytes para uma string e exibindo
            std::string data_string = bytes_to_string(data, result);
            std::cout << data_string;

            // Exibindo os dados em formato hexadecimal
            std::cout << " (Hex): ";
            for (int i = 0; i < result; ++i) {
                std::cout << std::hex << "0x" << static_cast<int>(data[i]) << " ";
            }
            std::cout << std::dec << std::endl; // Retornar ao formato decimal

        } else {
            std::cerr << "Erro ao ler dados do dispositivo: " << libusb_strerror(static_cast<libusb_error>(result)) << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(100));  // Atraso de 100ms
    }

    cleanup(handle, ctx);
    return 0;
}

