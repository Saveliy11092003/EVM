#include <iostream>
#include <libusb-1.0/libusb.h>
#include <stdio.h>
using namespace std;

void printdev(libusb_device *dev, ssize_t number);
int main(){
    libusb_device **devs; // указатель на указатель на устройство,
    // используется для получения списка устройств
    libusb_context *ctx = NULL; // контекст сессии libusb
    int r;                      // для возвращаемых значений
    ssize_t cnt;                // число найденных USB-устройств
    ssize_t i;                  // индексная переменная цикла перебора всех устройств
    // инициализировать библиотеку libusb, открыть сессию работы с libusb
    r = libusb_init(&ctx);
    if (r < 0){
        fprintf(stderr,
                "Ошибка: инициализация не выполнена, код: %d.\n", r);
        return 1;
    }
    // задать уровень подробности отладочных сообщений
        libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, 3);
    // получить список всех найденных USB- устройств
        cnt = libusb_get_device_list(ctx, &devs);
    if (cnt < 0) {
        fprintf(stderr, "Ошибка: список USB устройств не получен.\n");
        return 1;
    }
    int c = cnt;
    printf("найдено устройств: %d\n", c);

    for (i = 0; i < cnt; i++) {       // цикл перебора всех устройств
        printdev(devs[i], i); // печать параметров устройства
    }
    // освободить память, выделенную функцией получения списка устройств
    libusb_free_device_list(devs, 1);
    libusb_exit(ctx); // завершить работу с библиотекой libusb,
                  // закрыть сессию работы с libusb
        return 0;
}

void printdev(libusb_device *dev, ssize_t number){
    libusb_device_descriptor desc;    // дескриптор устройства
    libusb_config_descriptor *config; // дескриптор конфигурации объекта
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;

    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        fprintf(stderr,
                   "Ошибка: дескриптор устройства не получен, код: %d.\n", r);
        return;
    }
    
    libusb_device_handle *dev_handle;
    int length = 31;
    auto *data = new uint8_t[255];
    libusb_open(dev, &dev_handle);
    if (dev_handle != nullptr){
        if (libusb_get_string_descriptor_ascii(dev_handle, desc.iSerialNumber, data, 31) >= 0){
            cout << "Cерийный номер устройства " << number << ": " << data << endl;
        }
    }
    libusb_close(dev_handle);
    
    // получить конфигурацию устройства
    libusb_get_config_descriptor(dev, 0, &config);
    cout << "Количество возможных конфигураций устройства " << number << ": " << (int)desc.bNumConfigurations << endl;

    if ((int)desc.bDeviceClass == 9){
        cout << "Класс устройства " << number << ": " << "Концентратор" << endl;
    }
    else if ((int)desc.bDeviceClass == 224){
        cout << "Класс устройства " << number << ": " << "Беспроводной контроллер" << endl;
    }else if ((int)desc.bDeviceClass == 239){
        cout << "Класс устройства " << number << ": " << "Различные устройства" << endl;
    }else{
        cout << "Класс устройства " << number << ": " << (int)desc.bDeviceClass << endl;
    }

    cout << "Идентификатор производителя устройства " << number << ": " << desc.idVendor << endl;

    cout << "Идентификатор устройства " << number << ": " << desc.idProduct << endl;

    cout << "Количество интерфейсов конфигурации устройства " << number << ": " << (int)config->bNumInterfaces << endl;

    for (int i = 0; i < (int)config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        cout << "Количество альтернативных настроек интефейса " << i << " устройства " << number << ": " << inter->num_altsetting << endl;

        for (int j = 0; j < inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];

            cout << "Номер интерфейса устройства " << number << ": " << (int)interdesc->bInterfaceNumber << endl;

            cout << "Количество конечных точек интефейса " << i << " устройства " << number << ": " << (int)interdesc->bNumEndpoints << endl;

            for (int k = 0; k < (int)interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];

                cout << "Тип дескриптора конечной точки " << k << " интерфейса " << i << " устройства "  << number << ": " << (int)epdesc->bDescriptorType << endl;

                cout << "Адрес конечной точки  " << k << " интерфейса " << i << " устройства " << number << ": " << (int)(int)epdesc->bEndpointAddress << endl;
            }
        }
    }

    cout << "=======================================" << endl;

    libusb_free_config_descriptor(config);
}
