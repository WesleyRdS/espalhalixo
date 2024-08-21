#include <stdio.h>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>

#define BUFFER_SIZE 256

void list_usb_devices() {
    libusb_device_handle *handle;
    libusb_device **devs;
    libusb_context *ctx = NULL;
    ssize_t cnt;
    int r;

    r = libusb_init(&ctx);
    if (r < 0) {
        fprintf(stderr, "Não foi possível inicializar libusb: %s\n", libusb_strerror(r));
        exit(EXIT_FAILURE);
    }

    cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        fprintf(stderr, "Não foi possível obter a lista de dispositivos: %s\n", libusb_strerror((int)cnt));
        libusb_exit(ctx);
        exit(EXIT_FAILURE);
    }

    printf("Dispositivos USB conectados:\n");
    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device *device = devs[i];
        struct libusb_device_descriptor desc;
        int ret = libusb_get_device_descriptor(device, &desc);
        if (ret < 0) {
            fprintf(stderr, "Não foi possível obter a descrição do dispositivo: %s\n", libusb_strerror(ret));
            continue;
        }

        printf("ID do Produto: %04x, ID do Fornecedor: %04x\n", desc.idProduct, desc.idVendor);
        printf("Número de configurações: %d\n", desc.bNumConfigurations);
        // Adicione mais detalhes se necessário

    }

    libusb_free_device_list(devs, 1);
    libusb_exit(ctx);
}

int main() {
    list_usb_devices();
    return 0;
}

